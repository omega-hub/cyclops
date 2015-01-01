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
 *	A scene layer is an abstract class that groups entities together for a
 *  variety of purposes: lighting, clipping, LOD and so on. SceneLayers can form
 *	a hyerarchy similar to the scene node tree, but the scene layer tree is used
 *	to represent properties of the scene different than spatial transformations.
 ******************************************************************************/
#include "cyclops/SceneLayer.h"
#include "cyclops/Entity.h"
#include "cyclops/Light.h"

#include <osgUtil/CullVisitor>

using namespace omega;
using namespace cyclops;

const uint SceneLayer::CameraDrawExplicitLayers = 1 << 17;
const uint SceneLayer::Layer1 = 1 << 16;
const uint SceneLayer::Layer2 = 1 << 15;
const uint SceneLayer::Layer3 = 1 << 14;
const uint SceneLayer::Layer4 = 1 << 13;
const uint SceneLayer::Layer5 = 1 << 12;
const uint SceneLayer::Layer6 = 1 << 11;
const uint SceneLayer::Layer7 = 1 << 10;
const uint SceneLayer::Layer8 = 1 << 9;


///////////////////////////////////////////////////////////////////////////////
class LayerCullCallback: public osg::NodeCallback
{
public:
    LayerCullCallback(SceneLayer* sl) : myLayer(sl)
    {}

    // Callback method called by the NodeVisitor when visiting a node.*/
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        if(nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
        {
            // Do a traversal for each active material.
            osgUtil::CullVisitor* cv = (osgUtil::CullVisitor*)nv;

            // Retrieve the omegalib draw context from the osg cull visitor.
            omegaOsg::OsgDrawInformation* odi = 
                dynamic_cast<omegaOsg::OsgDrawInformation*>(cv->getRenderStage()->getCamera()->getUserData());

            Camera* activeCamera = NULL;
            if(odi != NULL)
            {
                activeCamera = odi->context->camera;
            }
			oassert(activeCamera != NULL);

            bool camExplicitLayers = activeCamera->isFlagSet(SceneLayer::CameraDrawExplicitLayers);
            // We draw this layer if:
            // 1 - the camera only draws layers associated to it, and this layer is
            // 2 - the layer has no associated camera
            // 3 - the layer is associated to this camera
            // (NEW IN 6.0) 4 - the layer id is an enabled camera flag
            if(myLayer->getCamera() == activeCamera ||
                (!camExplicitLayers && myLayer->getCamera() == NULL && myLayer->getId() == 0) ||
                activeCamera->isFlagSet(myLayer->getId()))
            {
                traverse(node, nv);
            }
        }
        else
        {
            // note, callback is responsible for scenegraph traversal so
            // they must call traverse(node,nv) to ensure that the
            // scene graph subtree (and associated callbacks) are traversed.
            traverse(node, nv);
        }
    }

private:
    SceneLayer* myLayer;
};

///////////////////////////////////////////////////////////////////////////////
SceneLayer::SceneLayer():
myParent(NULL), myId(0)
{
    myCullCallback = new LayerCullCallback(this);

    myRoot = new osg::Group();
    myRoot->addCullCallback(myCullCallback);
}

///////////////////////////////////////////////////////////////////////////////
SceneLayer::~SceneLayer()
{
    List< Entity* > tmpList = myEntities;
    // Detach all the attached entities.
    foreach(Entity* e, tmpList)
    {
        e->setLayer(NULL);
    }
    myEntities.clear();
}

///////////////////////////////////////////////////////////////////////////////
void SceneLayer::addEntity(Entity* e)
{
    oassert(e != NULL);
    e->addListener(this);
    myRoot->addChild(e->getOsgNode());
    myEntities.push_back(e);
}

///////////////////////////////////////////////////////////////////////////////
void SceneLayer::removeEntity(Entity* e)
{
    oassert(e != NULL);
    e->removeListener(this);
    myRoot->removeChild(e->getOsgNode());
    myEntities.remove(e);
}

///////////////////////////////////////////////////////////////////////////////
void SceneLayer::onAttachedToScene(SceneNode* source)
{
    // Called by entities when their parent node changes. Update the osg parent node
    // accordingly.
    Entity* e = dynamic_cast<Entity*>(source);
    if(e != NULL)
    {
        if(myRoot->getChildIndex(e->getOsgNode()) == myRoot->getNumChildren())
        {
            myRoot->addChild(e->getOsgNode());
        }
        else
        {
            owarn("SceneLayer::onAttachedToScene: trying to attach node twice");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void SceneLayer::onDetachedFromScene(SceneNode* source)
{
    // Called by entities when their parent node changes. Update the osg parent node
    // accordingly.
    Entity* e = dynamic_cast<Entity*>(source);
    if(e != NULL)
    {
        myRoot->removeChild(e->getOsgNode());
    }
}

///////////////////////////////////////////////////////////////////////////////
void SceneLayer::addLayer(SceneLayer* layer)
{
    if(layer != NULL)
    {
        if(layer->getParentLayer() != NULL)
        {
            layer->getParentLayer()->removeLayer(layer);
        }
        myLayers.push_back(layer);
        // Attach the layer osg node to this layer osg node
        myRoot->addChild(layer->getOsgNode());
    }
}

///////////////////////////////////////////////////////////////////////////////
void SceneLayer::removeLayer(SceneLayer* layer)
{
    if(layer != NULL)
    {
        layer->myParent = NULL;
        myLayers.remove(layer);
        // Detach the layer osg node to this layer osg node
        myRoot->removeChild(layer->getOsgNode());
    }
}

///////////////////////////////////////////////////////////////////////////////
void SceneLayer::update()
{
    updateLayer();
    foreach(SceneLayer* l, myLayers) l->update();
}
