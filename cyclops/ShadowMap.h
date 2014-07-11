/******************************************************************************
 * THE OMEGA LIB PROJECT
 *-----------------------------------------------------------------------------
 * Copyright 2010-2013		Electronic Visualization Laboratory, 
 *							University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-----------------------------------------------------------------------------
 * Copyright (c) 2010-2013, Electronic Visualization Laboratory,  
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
#ifndef __CY_SHADOW_MAP__
#define __CY_SHADOW_MAP__

#include "cyclopsConfig.h"
#include "ShadowMapGenerator.h"

#include <osg/Group>
#include <osgShadow/ShadowedScene>
#include <osgShadow/SoftShadowMap>

#define OMEGA_NO_GL_HEADERS
#include <omega.h>
#include <omegaOsg/omegaOsg.h>

namespace cyclops {
	using namespace omega;
	using namespace omegaOsg;

	class LightingLayer;
	class Light;

	///////////////////////////////////////////////////////////////////////////
	class ShadowMap: public ReferenceType
	{
	friend class Light;
	friend class LightingLayer;
	public:
		static const int ReceivesShadowTraversalMask = 0x1;
		static const int CastsShadowTraversalMask = 0x2;

	public:
		ShadowMap();
		osgShadow::ShadowedScene* getOsgNode()
		{ return myShadowedScene; }

		void setTextureUnit(int unit);
		int getTextureUnit();
		void setTextureSize(int width, int height);

		void setSoft(bool value);
		bool isSoft()
		{ return myShadowMap->isSoft(); }
		void setSoftShadowParameters(float softnessWidth, float jitteringScale)
		{ return myShadowMap->setSoftShadowParameters(softnessWidth, jitteringScale); }

		void setDirty() 
		{ myShadowMap->setDirty(); }
        
	private:
		//! used by Light to notify tell this shadow map who is its owner.
		void setLight(Light* l); 

		// Attaches this shadow map to the specified layer
		void setLayer(LightingLayer* layer);
		void addToLayer(LightingLayer* layer);
		void removeFromLayer(LightingLayer* layer);
		void setManualRefreshEnabled(bool value) 
		{ myShadowMap->setManualRefreshEnabled(value); }
		
	protected:
		virtual void initialize();
		void checkInitialized();

	protected:
		bool myInitialized;
		Light* myLight;
		int myShadowTextureUnit;
		LightingLayer* myLayer;
        Ref<ShadowMapGenerator> myShadowMap;
		Ref<osgShadow::ShadowedScene> myShadowedScene;
	};

	///////////////////////////////////////////////////////////////////////////
	/*class SoftShadowMap: public ShadowMap
	{
	protected:
		virtual void initialize();

	private:
        Ref<osgShadow::SoftShadowMap> mySoftShadowMap;
	};*/
};

#endif
