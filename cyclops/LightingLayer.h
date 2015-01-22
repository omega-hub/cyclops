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
#ifndef __CY_LIGHTING_LAYER__
#define __CY_LIGHTING_LAYER__

#include "SceneLayer.h"
#include "ShaderManager.h"
#include "Light.h"

namespace cyclops {
	///////////////////////////////////////////////////////////////////////////
	class CY_API LightingLayer: public SceneLayer
	{
	friend class Light;
	public:
		typedef Dictionary<Light*, LightInstance* > LightInstanceMap;

	public:
		//! Constructs a new Lighting layer, using the passed shader manager
		LightingLayer(ShaderManager* sm);
		//! Constructs a new Lighting layer, creating a shader manager internally.
		LightingLayer();
		~LightingLayer();

		//! Add a sub-layer. Lights applied to this layer will be also
		//! be applied to all lighting sub-layers. 
		virtual void addLayer(SceneLayer* layer);
		virtual void removeLayer(SceneLayer* layer);

		virtual osg::Group* getOsgNode() { return myPreShadowNode; }
		virtual osg::Group* getPostShadowOsgNode() { return myRoot; }

		//! Given a light, find a corresponding light instance attached to this
		//! layer. Returns NULL if no instance is found.
		LightInstance* findLightInstance(Light* l);

		//! @internal SceneNodeListener overrides
		//! These methods are needed to handle entities when they get attached
		//! or detached from the scene
		virtual void onAttachedToScene(SceneNode* source);
		virtual void onDetachedFromScene(SceneNode* source);

		ShaderManager* getShaderManager() { return myShaderManager; }

	protected:
		//! This methods are never used directly but are called by Light::setLayer
		virtual void addLight(Light* l);
		virtual void removeLight(Light* l);

		virtual void updateLayer();
		// Reimplemented, so we can tell Entities to use the layer shader manager
		virtual void addEntity(Entity* e);

		void addLightToSubLayers(SceneLayer* layer, Light* l);
		void removeLightFromSubLayers(SceneLayer* layer, Light* l);

	private:
		LightInstanceMap myLights;
		ShaderManager* myShaderManager;
		
		// This is the node over which shadowed scenes are applied.
		Ref<osg::Group> myPreShadowNode;
	};

	///////////////////////////////////////////////////////////////////////////
	inline LightInstance* LightingLayer::findLightInstance(Light* l)
	{
		if(myLights.find(l) != myLights.end()) return myLights[l];
		return NULL;
	}
};


#endif