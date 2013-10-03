/**************************************************************************************************
 * THE OMEGA LIB PROJECT
 *-------------------------------------------------------------------------------------------------
 * Copyright 2010-2013		Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-------------------------------------------------------------------------------------------------
 * Copyright (c) 2010-2013, Electronic Visualization Laboratory, University of Illinois at Chicago
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
#include "cyclops/Light.h"
#include "cyclops/SceneManager.h"

using namespace cyclops;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Light* Light::create()
{
	return new Light(SceneManager::instance());
}


///////////////////////////////////////////////////////////////////////////////////////////////
Light::Light(SceneManager* scene):
	SceneNode(scene->getEngine()),
	mySceneManager(scene),
	myColor(Color::White),
	myAmbient(Color::Black),
	myAttenuation(Vector3f(1.0, 0.0, 0.0)),
	myEnabled(false),
	mySoftShadowWidth(0.005f),
	mySoftShadowJitter(32),
	myOsgLight(NULL), myOsgLightSource(NULL),
	myDirty(false),
	mySpotCutoff(0),
	mySpotExponent(1)
{
	mySceneManager->addLight(this);
	setLightType(Point);
}

///////////////////////////////////////////////////////////////////////////////////////////////
Light::~Light()
{}

///////////////////////////////////////////////////////////////////////////////////////////////
void Light::setLightType(LightType type)
{
	myType = type;
	switch(myType)
	{
	case Point: myLightFunction = "pointLightFunction"; break;
	case Directional: myLightFunction = "directionalLightFunction"; break;
	case Spot: myLightFunction = "spotLightFunction"; break;
	}
	myDirty = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
bool Light::updateOsgLight(int lightId, osg::Group* rootNode)
{
	if(myEnabled)
	{
		if(myOsgLight == NULL)
		{
			myOsgLight = new osg::Light();
			myOsgLightSource = new osg::LightSource();
			rootNode->addChild(myOsgLightSource);
		}

		// If the attachment node is different than the one the lightsource is attached to, change it.
		if(rootNode != myOsgLightSource->getParent(0))
		{
			myOsgLightSource->getParent(0)->removeChild(myOsgLightSource);
			rootNode->addChild(rootNode);
		}

		osg::Light* ol = myOsgLight;
		osg::LightSource* ols = myOsgLightSource;
		const Vector3f pos = getDerivedPosition();

		ol->setLightNum(lightId);
		ol->setPosition(osg::Vec4(pos[0], pos[1], pos[2], 1.0));
		ol->setAmbient(COLOR_TO_OSG(myAmbient));
		ol->setDiffuse(COLOR_TO_OSG(myColor));
		ol->setSpecular(COLOR_TO_OSG(myColor));
		ol->setConstantAttenuation(myAttenuation[0]);
		ol->setLinearAttenuation(myAttenuation[1]);
		ol->setQuadraticAttenuation(myAttenuation[2]);
		ol->setDirection(osg::Vec3(myLightDirection[0], myLightDirection[1], myLightDirection[2]));
		ol->setSpotCutoff(mySpotCutoff);
		ol->setSpotExponent(mySpotExponent);

		ols->setLight(ol);

		osg::StateSet* sState = rootNode->getOrCreateStateSet();
		ols->setStateSetModes(*sState,osg::StateAttribute::ON);
	}
	else
	{
		if(myOsgLightSource != NULL)
		{
			osg::StateSet* sState = rootNode->getOrCreateStateSet();
			myOsgLightSource->setStateSetModes(*sState,osg::StateAttribute::OFF); 
			myOsgLightSource->setLight(NULL);
		}
	}

	bool dirty = myDirty;
	myDirty = false;
	return dirty;
}

