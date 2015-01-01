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
 ******************************************************************************/
#include "cyclops/LightingLayer.h"
#include "cyclops/Entity.h"

using namespace omega;
using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////
LightingLayer::LightingLayer():
    myShaderManager(new ShaderManager())
{
    myPreShadowNode = new osg::Group();
    myPreShadowNode->addChild(myRoot);
}

///////////////////////////////////////////////////////////////////////////////
LightingLayer::LightingLayer(ShaderManager* sm):
    myShaderManager(sm)
{
    myPreShadowNode = new osg::Group();
    myPreShadowNode->addChild(myRoot);
}

///////////////////////////////////////////////////////////////////////////////
LightingLayer::~LightingLayer()
{
}

///////////////////////////////////////////////////////////////////////////////
void LightingLayer::addEntity(Entity* e)
{
    SceneLayer::addEntity(e);
    e->setShaderManager(myShaderManager);
}

///////////////////////////////////////////////////////////////////////////////
void LightingLayer::addLight(Light* l)
{
    oassert(l != NULL);
    LightInstance* li = l->createInstance(myRoot);
    myLights[l] = li;
    myShaderManager->addLightInstance(li);
    addLightToSubLayers(this, l);
    l->addListener(this);
}

///////////////////////////////////////////////////////////////////////////////
void LightingLayer::removeLight(Light* l)
{
    if(myLights.find(l) != myLights.end())
    {
        LightInstance* li = myLights[l];
        myLights.erase(l);
        myShaderManager->removeLightInstance(li);
        l->destroyInstance(li);
        removeLightFromSubLayers(this, l);
        l->removeListener(this);
    }
}

///////////////////////////////////////////////////////////////////////////////
void LightingLayer::onAttachedToScene(SceneNode* source)
{
    Light* l = dynamic_cast<Light*>(source);
    if(l != NULL)
    {
        LightInstance* li = l->createInstance(myRoot);
        myLights[l] = li;
        myShaderManager->addLightInstance(li);
        if(l->getShadow())
        {
            l->getShadow()->setLayer(this);
        }
    }
    SceneLayer::onAttachedToScene(source);
}

///////////////////////////////////////////////////////////////////////////////
void LightingLayer::onDetachedFromScene(SceneNode* source)
{
    Light* l = dynamic_cast<Light*>(source);
    if(l != NULL)
    {
        if(myLights.find(l) != myLights.end())
        {
            LightInstance* li = myLights[l];
            myLights.erase(l);
            myShaderManager->removeLightInstance(li);
            l->destroyInstance(li);
            if(l->getShadow())
            {
                l->getShadow()->setLayer(NULL);
            }
        }
    }
    SceneLayer::onDetachedFromScene(source);
}

///////////////////////////////////////////////////////////////////////////////
void LightingLayer::addLayer(SceneLayer* layer)
{
    SceneLayer::addLayer(layer);
    // Add all of the lights of this layer to all lighting sublayers.
    LightingLayer* ll = dynamic_cast<LightingLayer*>(layer);
    typedef KeyValue<Light*, LightInstance*> LightInstanceMapItem;
    foreach(LightInstanceMapItem i, myLights)
    {
        if(ll != NULL) ll->addLight(i.getKey());
        else addLightToSubLayers(layer, i.getKey());
    }
}

///////////////////////////////////////////////////////////////////////////////
void LightingLayer::removeLayer(SceneLayer* layer)
{
    SceneLayer::removeLayer(layer);
    // Add all of the lights of this layer to all lighting sublayers.
    LightingLayer* ll = dynamic_cast<LightingLayer*>(layer);
    typedef KeyValue<Light*, LightInstance*> LightInstanceMapItem;
    foreach(LightInstanceMapItem i, myLights)
    {
        if(ll != NULL) ll->addLight(i.getKey());
        else removeLightFromSubLayers(layer, i.getKey());
    }
}

///////////////////////////////////////////////////////////////////////////////
void LightingLayer::addLightToSubLayers(SceneLayer* layer, Light* l)
{
    foreach(SceneLayer* sl, layer->getLayers())
    {
        LightingLayer* ll = dynamic_cast<LightingLayer*>(sl);
        if(ll != NULL) ll->addLight(l);
        else addLightToSubLayers(sl, l);
    }
}

///////////////////////////////////////////////////////////////////////////////
void LightingLayer::removeLightFromSubLayers(SceneLayer* layer, Light* l)
{
    foreach(SceneLayer* sl, layer->getLayers())
    {
        LightingLayer* ll = dynamic_cast<LightingLayer*>(sl);
        if(ll != NULL) ll->removeLight(l);
        else removeLightFromSubLayers(sl, l);
    }
}

///////////////////////////////////////////////////////////////////////////////
void LightingLayer::updateLayer()
{
    myShaderManager->update();
}
