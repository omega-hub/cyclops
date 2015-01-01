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
 *	A light that can be added to a cyclops scene.
 ******************************************************************************/
#include "cyclops/Light.h"
#include "cyclops/SceneManager.h"
#include "cyclops/LightingLayer.h"

using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////
Light* Light::create()
{
    return new Light(SceneManager::instance());
}

///////////////////////////////////////////////////////////////////////////////
Light::Light(SceneManager* scene):
    SceneNode(scene->getEngine()),
    mySceneManager(scene),
    myColor(Color::White),
    myAmbient(Color::Black),
    myAttenuation(Vector3f(1.0, 0.0, 0.0)),
    myEnabled(true),
    // NOTE: for non-spot lights FOV needs to be > of 180 otherwise shadow maps
    // will try to use a non-existent light direction vector so setup a shadow
    // map, causing errors.
    mySpotCutoff(180),
    mySpotExponent(1),
    myLayer(NULL),
    myShadowRefreshMode(OnFrame)
{
    getEngine()->getScene()->addChild(this);
    setLayer(mySceneManager->getLightingLayer());
    setLightType(Point);
}

///////////////////////////////////////////////////////////////////////////////
Light::~Light()
{
    setLayer(NULL);
}

///////////////////////////////////////////////////////////////////////////////
void Light::updateTraversal(const UpdateContext& context)
{
    if(myShadow != NULL && myShadowRefreshMode == OnLightMove)
    {
        if(myLastShadowPos != getDerivedPosition())
        {
            myShadow->setDirty();
            myLastShadowPos = getDerivedPosition();
        }
    }
    SceneNode::updateTraversal(context);
}

///////////////////////////////////////////////////////////////////////////////
void Light::setEnabled(bool value)
{
    if(myEnabled != value)
    {
        myEnabled = value;
        if(myShadow != NULL)
        {
            if(myEnabled)
            {
                myShadow->setLayer(myLayer);
            }
            else
            {
                myShadow->setLayer(NULL);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Light::setShadow(ShadowMap* s)
{
    if(myShadow != NULL) myShadow->setLayer(NULL);
    myShadow = s;
    if(myShadow != NULL)
    {
        s->setLight(this);
        s->setLayer(myLayer);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Light::setShadowRefreshMode(ShadowRefreshMode srm)
{
    myShadowRefreshMode = srm;
    if(myShadow != NULL)
    {
        if(myShadowRefreshMode != OnFrame)
        {
            myShadow->setManualRefreshEnabled(true);
        }
        else
        {
            myShadow->setManualRefreshEnabled(false);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Light::setLayer(LightingLayer* layer)
{
    if(myLayer != NULL) myLayer->removeLight(this);
    if(layer != NULL) layer->addLight(this);
    myLayer = layer;
    if(myShadow != NULL) myShadow->setLayer(myLayer);
}

///////////////////////////////////////////////////////////////////////////////
LightingLayer* Light::getLayer()
{
    return myLayer;
}

///////////////////////////////////////////////////////////////////////////////
void Light::setLightType(LightType type)
{
    myType = type;
    switch(myType)
    {
    case Point: myLightFunction = "pointLightFunction"; break;
    case Directional: myLightFunction = "directionalLightFunction"; break;
    case Spot: myLightFunction = "spotLightFunction"; break;
    }
    requestShaderUpdate();
}

///////////////////////////////////////////////////////////////////////////////
LightInstance* Light::createInstance(osg::Group* rootNode)
{
    LightInstance* li = new LightInstance(this, rootNode);
    myInstances.push_back(li);
    return li;
}

///////////////////////////////////////////////////////////////////////////////
void Light::destroyInstance(LightInstance* i)
{
    if(i != NULL)
    {
        myInstances.remove(i);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Light::requestShaderUpdate()
{
    foreach(LightInstance* li, myInstances)
    {
        li->requestShaderUpdate();
    }
}

///////////////////////////////////////////////////////////////////////////////
LightInstance::LightInstance(Light* l, osg::Group* root):
    myLight(l),
    myGroup(root),
    myShaderUpdateNeeded(true),
    myIndex(0)
{
    myOsgLight = new osg::Light();
    myOsgLightSource = new osg::LightSource();
    myGroup->addChild(myOsgLightSource);
}

///////////////////////////////////////////////////////////////////////////////
LightInstance::~LightInstance()
{
    myGroup->removeChild(myOsgLightSource);
}

///////////////////////////////////////////////////////////////////////////////
void LightInstance::setLightIndex(int index)
{
    myIndex = index;
    myOsgLight->setLightNum(myIndex);
}

///////////////////////////////////////////////////////////////////////////////
bool LightInstance::update()
{
    if(myLight->myEnabled)
    {
        osg::Light* ol = myOsgLight;
        osg::LightSource* ols = myOsgLightSource;
        const Vector3f pos = myLight->getDerivedPosition();

        if(myLight->myType != Light::Directional)
        {
            ol->setPosition(osg::Vec4(pos[0], pos[1], pos[2], 1.0));
        }
        else
        {
            ol->setPosition(osg::Vec4(pos[0], pos[1], pos[2], 0.0));
        }
        ol->setAmbient(COLOR_TO_OSG(myLight->myAmbient));
        ol->setDiffuse(COLOR_TO_OSG(myLight->myColor));
        ol->setSpecular(COLOR_TO_OSG(myLight->myColor));
        ol->setConstantAttenuation(myLight->myAttenuation[0]);
        ol->setLinearAttenuation(myLight->myAttenuation[1]);
        ol->setQuadraticAttenuation(myLight->myAttenuation[2]);

        // Re-orient light direction based on light node orientation.
        Vector3f lightDir = myLight->getDerivedOrientation() * myLight->myLightDirection;

        ol->setDirection(osg::Vec3(lightDir[0], lightDir[1], lightDir[2]));
        ol->setSpotCutoff(myLight->mySpotCutoff);
        ol->setSpotExponent(myLight->mySpotExponent);

        ols->setLight(ol);

        osg::StateSet* sState = myGroup->getOrCreateStateSet();
        ols->setStateSetModes(*sState,osg::StateAttribute::ON);
    }
    else
    {
        osg::StateSet* sState = myGroup->getOrCreateStateSet();
        myOsgLightSource->setStateSetModes(*sState,osg::StateAttribute::OFF); 
        myOsgLightSource->setLight(NULL);
    }

    bool update = myShaderUpdateNeeded;
    myShaderUpdateNeeded = false;
    return update;
}
