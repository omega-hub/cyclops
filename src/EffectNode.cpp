/******************************************************************************
 * THE OMEGA LIB PROJECT
 *-----------------------------------------------------------------------------
 * Copyright 2010-2015		Electronic Visualization Laboratory, 
 *							University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-----------------------------------------------------------------------------
 * Copyright (c) 2010-2015, Electronic Visualization Laboratory,  
 * University of Illinois at Chicago
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this 
 * list of conditions and the following disclaimer. Redistributions in binary 
 * form must reproduce the above copyright notice, this list of conditions and 
 * the following disclaimer in the documentation and/or other materials provided 
 * with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *-----------------------------------------------------------------------------
 * What's in this file
 *	An osg node handling visual effects applied to entities. This class is for
 *  internal use only.
 ******************************************************************************/
#include "cyclops/EffectNode.h"
#include "cyclops/SceneManager.h"

#include <osgFX/Technique>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include<osg/BlendFunc>

using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////
EffectNode::EffectNode(SceneManager* sm):
    mySceneManager(sm)
{
}

///////////////////////////////////////////////////////////////////////////////
EffectNode::~EffectNode() 
{
}

///////////////////////////////////////////////////////////////////////////////
Material* EffectNode::getMaterial(unsigned int index)
{
    List< Ref<Material> >::iterator it = myMaterials.begin();
    std::advance(it, index);
    return *it;
}

///////////////////////////////////////////////////////////////////////////////
void EffectNode::addMaterial(Material* mat)
{
    myMaterials.push_back(mat);
}

///////////////////////////////////////////////////////////////////////////////
void EffectNode::removeMaterial(Material* mat)
{
    myMaterials.remove(mat);
}

///////////////////////////////////////////////////////////////////////////////
void EffectNode::clearMaterials()
{
    myMaterials.clear();
}

///////////////////////////////////////////////////////////////////////////////
void EffectNode::setDefinition(const String& definition)
{
    myDefinition = definition;
    
    // If an effect definition is specified, clear the effect materials and recreate them use
    // the effect definition string.
    if(myDefinition != "")
    {
        clearMaterials();

        Vector<String> passes = StringUtils::split(myDefinition, "|");

        for(Vector<String>::iterator iter = passes.begin(); iter != passes.end(); iter++)
        {
            StringUtils::trim(*iter);
            osg::StateSet* ss = new osg::StateSet();
            Material* mat = new Material(ss, mySceneManager);
            if(mat->parse(*iter)) 
            {
                addMaterial(mat);

                // Renrer bin hack: addPass resets the render bin for the stateset, so we force
                // the opaque / transparent renderbin settings here. Re-calling setTransparent
                // will make sure the render bin is correct.
                // NOTE: After stopping use of osgFX we probably don't need
                // this anymore...
                mat->setTransparent(mat->isTransparent());
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void EffectNode::setShaderManager(ShaderManager* sm)
{
    myShaderManager = sm;
    foreach(Material* m, myMaterials)
    {
        m->setShaderManager(sm);
    }
}

///////////////////////////////////////////////////////////////////////////////
void EffectNode::traverse(osg::NodeVisitor& nv)
{
    if(nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
    {
        // Do a traversal for each active material.
        osgUtil::CullVisitor* cv = (osgUtil::CullVisitor*)&nv;

        // Retrieve the omegalib draw context from the osg cull visitor.
        omegaOsg::OsgDrawInformation* odi = 
            dynamic_cast<omegaOsg::OsgDrawInformation*>(cv->getRenderStage()->getCamera()->getUserData());

        Camera* activeCamera = NULL;
        if(odi != NULL)
        {
            activeCamera = odi->context->camera;
        }

        bool camExplicitMaterials = activeCamera->isFlagSet(Material::CameraDrawExplicitMaterials);

        foreach(Material* mat, myMaterials)
        {
            // We draw this material if:
            // 1 - the camera only draws materials associated to it, and this material is
            // 2 - the material has no associated camera
            // 3 - the material is associated to this camera
            if(mat->getCamera() == activeCamera ||
                (!camExplicitMaterials && mat->getCamera() == NULL))
            {
                cv->pushStateSet(mat->getStateSet());
                Group::traverse(nv);
                cv->popStateSet();
            }
        }
    }
    else
    {
        Group::traverse(nv);
    }
}
