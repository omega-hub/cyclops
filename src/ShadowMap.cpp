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
#include "cyclops/Light.h"
#include "cyclops/SceneManager.h"
#include "cyclops/LightingLayer.h"
#include "cyclops/ShadowMap.h"

using namespace cyclops;


///////////////////////////////////////////////////////////////////////////////
ShadowMap::ShadowMap():
	myLayer(NULL),
	myLight(NULL),
	myInitialized(false),
	myShadowTextureUnit(-1)
{
	myShadowedScene = new osgShadow::ShadowedScene();
	myShadowedScene->setReceivesShadowTraversalMask(ShadowMap::ReceivesShadowTraversalMask);
	myShadowedScene->setCastsShadowTraversalMask(ShadowMap::CastsShadowTraversalMask);
    initialize();
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMap::checkInitialized()
{
	if(!myInitialized)
	{
		initialize();
	}
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMap::setLight(Light* l) 
{ 
	checkInitialized(); 
	myLight = l; 
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMap::setLayer(LightingLayer* layer)
{
	if(myLayer != NULL) removeFromLayer(myLayer);
	if(layer != NULL) addToLayer(layer);
	myLayer = layer;
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMap::addToLayer(LightingLayer* layer)
{
	oassert(layer);
	ShadowMap* sm = myLight->getShadow();
	oassert(sm);

	layer->getOsgNode()->addChild(sm->getOsgNode());
	sm->getOsgNode()->addChild(layer->getPostShadowOsgNode());

	LightInstance* li = layer->findLightInstance(myLight);
	oassert(li);
	myShadowMap->setLight(li->getOsgLight());
	myShadowMap->setShadowedSceneStateSet(layer->getPostShadowOsgNode()->getOrCreateStateSet());
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMap::removeFromLayer(LightingLayer* layer)
{
	oassert(layer);

	ShadowMap* sm = myLight->getShadow();
	oassert(sm);
	layer->getOsgNode()->removeChild(sm->getOsgNode());
	sm->getOsgNode()->removeChild(layer->getPostShadowOsgNode());

	myShadowTextureUnit = -1;
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMap::setTextureUnit(int unit)
{
	checkInitialized();
	myShadowTextureUnit = unit;
	//ofmsg("Setting shadow for light %1% to %2%", %myLight->getName() %unit);
	myShadowMap->setTextureUnit(unit);
}

///////////////////////////////////////////////////////////////////////////////
int ShadowMap::getTextureUnit()
{
	checkInitialized();
	return myShadowTextureUnit;
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMap::setTextureSize(int width, int height)
{
	checkInitialized(); 
	myShadowMap->setTextureSize(osg::Vec2s(width, height));
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMap::initialize()
{
    myShadowMap = new ShadowMapGenerator();
	//myShadowMap->setTextureUnit(7);
    myShadowedScene->setShadowTechnique(myShadowMap);
	myShadowMap->setTextureSize(osg::Vec2s(512, 512));
	myInitialized = true;
}

void ShadowMap::setSoft(bool value)
{ 
    if(myShadowMap->isSoft() != value)
    {
        myShadowMap->setSoft(value); 
        if(myLight != NULL) myLight->requestShaderUpdate();
    }
}

///////////////////////////////////////////////////////////////////////////////
//void SoftShadowMap::initialize()
//{
//    mySoftShadowMap = new osgShadow::SoftShadowMap;
//    // Hardcoded ambient bias for shadow map. Shadowed areas receive zero light. 
//    // Unshadowed areas receive full light.
//    mySoftShadowMap->setAmbientBias(osg::Vec2(0.0f, 1.0f));
//    // Hardcoded texture unit arguments for shadow map.
//	mySoftShadowMap->setSoftnessWidth(0.005f);
//    mySoftShadowMap->setJitteringScale(32);
//	mySoftShadowMap->setTextureUnit(4);
//    mySoftShadowMap->setJitterTextureUnit(5);
//    myShadowedScene->setShadowTechnique(mySoftShadowMap);
//	mySoftShadowMap->setTextureSize(osg::Vec2s(512, 512));
//	myShadowMap = mySoftShadowMap;
//}