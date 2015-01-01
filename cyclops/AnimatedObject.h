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
* What's in this file:
* A class to handle entities that contain animations.
******************************************************************************/
#ifndef __CY_ANIMATED_OBJECT__
#define __CY_ANIMATED_OBJECT__

#include <osg/Texture2D>
#include <osg/Light>
#include <osg/Group>
#include <osg/Switch>
#include <osgShadow/ShadowedScene>
#include <osgShadow/SoftShadowMap>
#include <osgAnimation/BasicAnimationManager>

#define OMEGA_NO_GL_HEADERS
#include <omega.h>
#include <omegaOsg/omegaOsg.h>
#include <omegaToolkit.h>

#include "cyclopsConfig.h"
#include "Skybox.h"
#include "SceneManager.h"
#include "Entity.h"

namespace cyclops {
	using namespace omega;
	using namespace omegaOsg;

	class SceneManager;

	///////////////////////////////////////////////////////////////////////////
	//! Represents an object with embedded animations.
	class CY_API AnimatedObject: public Entity
	{
	public:
		//! #PYAPI Convenience method to create a new AnimatedObject instance.
		static AnimatedObject* create(const String& modelName);
		static AnimatedObject* fromEntity(Entity* dobj);

	public:
		AnimatedObject(SceneManager* mng, const String& modelName);
		virtual ~AnimatedObject() {}

		virtual void updateTraversal(const UpdateContext& context);

		ModelAsset* getModel();

		int getNumModels() { return myModel->numNodes; }
		void setCurrentModelIndex(int index);
		int getCurrentModelIndex();

		//! Animation support
		//@{
		bool hasAnimations();
		int getNumAnimations();
		void playAnimation(int id);
		void loopAnimation(int id);
		void pauseAnimation();
		void stopAllAnimations();
		int getCurAnimation();
		float getAnimationLength(int id);
		float getAnimationStart(int id);
		void setAnimationStart(int id, float time);
		//@}

		void setOnAnimationEndedScript(const String& value) { myOnAnimationEnded = value; }
		String getOnAnimationEndedScript() { return myOnAnimationEnded; }

	private:
		SceneManager* mySceneManager;

		ModelAsset* myModel;
		osg::Switch* myOsgSwitch;
		int myCurrentModelIndex;

		// osg animation stuff
		osgAnimation::BasicAnimationManager* myAnimationManager;
		const osgAnimation::AnimationList* myAnimations;
		int myCurAnimationId;
		osgAnimation::Animation* myCurAnimation;
		bool myNeedStartAnimation;

		float myCurTime;

		// Event handler scripts
		String myOnAnimationEnded;
	};

	///////////////////////////////////////////////////////////////////////////
	inline ModelAsset* AnimatedObject::getModel()
	{ return myModel; }

	///////////////////////////////////////////////////////////////////////////
	inline AnimatedObject* AnimatedObject::fromEntity(Entity* dobj)
	{ return dynamic_cast<AnimatedObject*>(dobj); }

};

#endif
