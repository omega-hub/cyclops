/**************************************************************************************************
 * THE OMEGA LIB PROJECT
 *-------------------------------------------------------------------------------------------------
 * Copyright 2010-2015		Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-------------------------------------------------------------------------------------------------
 * Copyright (c) 2010-2015, Electronic Visualization Laboratory, University of Illinois at Chicago
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted 
 * provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this list of conditions 
 * and the following disclaimer. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the documentation and/or other 
 * materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF 
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************************************/
#include <osgUtil/Optimizer>
#include <osgDB/Archive>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgAnimation/Animation>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/TangentSpaceGenerator>

#include "cyclops/ModelLoader.h"
#include "cyclops/AnimatedObject.h"

using namespace cyclops;
using namespace omegaToolkit;
using namespace omegaToolkit::ui;

// Default attribute binding for tangent array.
int DefaultTangentAttribBinding = 6;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct AnimationManagerFinder : public osg::NodeVisitor 
{ 
    osgAnimation::BasicAnimationManager* am; 
    
    AnimationManagerFinder(): am(NULL) {setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN); } 
    
    void apply(osg::Node& node) 
    { 
        if (am != NULL) 
            return; 
    
        if (node.getUpdateCallback())
        {        
            am = dynamic_cast<osgAnimation::BasicAnimationManager*>(node.getUpdateCallback()); 
            return; 
        } 
        
        traverse(node); 
    } 
}; 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextureResizeNonPowerOfTwoHintVisitor : public osg::NodeVisitor
{
protected:
    void setHint(osg::StateSet * stateset)
    {
        //TODO: get max texture units from somewhere
        for(int i = 0; i < 32; i++)
        {
            osg::StateAttribute* stateatt = stateset->getTextureAttribute(i, osg::StateAttribute::TEXTURE);
            if(stateatt)
            {
                osg::Texture * texture = stateatt->asTexture();
                if(texture)	texture->setResizeNonPowerOfTwoHint(_hint);
            }
        }
    }        
    bool _hint;

public:
    TextureResizeNonPowerOfTwoHintVisitor(bool hint): osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
    { _hint = hint;	}

    ~TextureResizeNonPowerOfTwoHintVisitor()
    {}

    virtual void apply(osg::Node& node)
    {
        osg::StateSet * stateset = node.getOrCreateStateSet();
        if(stateset) setHint(stateset);
        traverse(node);
    }

    virtual void apply(osg::Geode& node)
    {
        osg::StateSet * stateset = node.getOrCreateStateSet();
        if(stateset) setHint(stateset);
    
        for(int i = 0; i < node.getNumDrawables(); i++)
        {
            stateset = node.getDrawable(i)->getStateSet();
            if(stateset) setHint(stateset);
        }
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TangentSpaceGeneratorVisitor: public osg::NodeVisitor
{
protected:
    int tangentAttribBinding;

public:
    TangentSpaceGeneratorVisitor(int tangentAttribBinding = DefaultTangentAttribBinding): osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
    { this->tangentAttribBinding = tangentAttribBinding; }

    ~TangentSpaceGeneratorVisitor()
    {}

    virtual void apply(osg::Geode& node)
    {
        for(int i = 0; i < node.getNumDrawables(); i++)
        {
            osg::Geometry* geom = node.getDrawable(i)->asGeometry();
            if(geom != NULL)
            {
                Ref<osgUtil::TangentSpaceGenerator> tsg = new osgUtil::TangentSpaceGenerator();
                tsg->generate(geom, 0);
                osg::Vec4Array* a_tangent = tsg->getTangentArray();
                geom->setVertexAttribArray (tangentAttribBinding, a_tangent);
                geom->setVertexAttribBinding (tangentAttribBinding, osg::Geometry::BIND_PER_VERTEX);
            }
        }
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
osg::Node* ModelLoader::processDefaultOptions(osg::Node* node, ModelAsset* asset)
{
    if(node != NULL)
    {
        if(asset->info->optimize)
        {
            ofmsg("Optimizing model...%1%", %asset->info->path);
            osgUtil::Optimizer optOSGFile;
            optOSGFile.optimize(node, 
                osgUtil::Optimizer::SHARE_DUPLICATE_STATE | 
                osgUtil::Optimizer::MERGE_GEOMETRY | 
                osgUtil::Optimizer::SPATIALIZE_GROUPS |
                osgUtil::Optimizer::MERGE_GEODES |
                // NOTE: TRISTRIP_GEOMETRY makes performance worse on some 
                // models (tested with FBX files). so it is commented out here.
                // If it turns out it is useful for other ill-formed models,
                // it should be added as an additional option to the model
                // loader.
                // Also check this post
                // https://groups.google.com/forum/#!msg/omegalib/4kDO3DfP96U/_fH9DmQ7zWAJ
                // TL;DR - TRISTRIP_GEOMETRY is useless and VERTEX_POSTTRANSFORM
                // is better and should be used instead.
                //osgUtil::Optimizer::TRISTRIP_GEOMETRY |
                osgUtil::Optimizer::VERTEX_POSTTRANSFORM);
        }

        if(asset->info->usePowerOfTwoTextures)
        {
            TextureResizeNonPowerOfTwoHintVisitor potv(true);
            node->accept(potv);
        }
        else
        {
            TextureResizeNonPowerOfTwoHintVisitor potv(false);
            node->accept(potv);
        }

        if(asset->info->size != 0.0f)
        {
            float r = node->getBound().radius() * 2;
            float scale = asset->info->size / r;

            osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform();
            pat->setScale(osg::Vec3(scale, scale, scale));
            pat->addChild(node);

            node = pat;
        }

        if(asset->info->generateNormals)
        {
            omsg("Generating normals...");
            osgUtil::SmoothingVisitor sv;
            node->accept(sv);
        }

        if(asset->info->generateTangents)
        {
            omsg("Generating tangents...");
            TangentSpaceGeneratorVisitor tsgv;
            node->accept(tsgv);
        }

        if(asset->info->normalizeNormals)
        {
            node->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON); 
        }

        asset->nodes.push_back(node);
        //asset->description = asset->info->description;
    }
    return node;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DefaultModelLoader::load(ModelAsset* asset)
{
    String orfp = StringUtils::replaceAll(asset->name, "*", "%1%");
    String filePath = asset->info->path;

    for(int iterator=0; iterator < asset->numNodes; iterator++)
    {
        // If present in the string, this line will substitute %1% with the iterator number.
        if(asset->numNodes != 1)
        {
            filePath = ostr(orfp, %iterator);
        }

        String assetPath;
        if(DataManager::findFile(filePath, assetPath))
        { 
            ofmsg("Loading model......%1%", %filePath);
            Ref<osgDB::Options> options = new osgDB::Options; 
            options->setOptionString("noTesselateLargePolygons noTriStripPolygons noRotation");

            if(asset->info->buildKdTree)
            {
                osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::ReaderWriter::Options::BUILD_KDTREES);
            }
            else
            {
                osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::ReaderWriter::Options::DO_NOT_BUILD_KDTREES);
            }

            osg::Node* node = osgDB::readNodeFile(assetPath, options);
            if(node != NULL)
            {
                node = processDefaultOptions(node, asset);
                asset->nodes.push_back(node);
            }
            else
            {
                //ofwarn("loading failed: %1%", %assetPath);
                return false;
            }
        }
        else
        {
            ofwarn("could not find file: %1%", %filePath);
            return false;
        }
    }
    return true;
}
