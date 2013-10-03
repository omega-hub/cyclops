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
#ifndef __CY_EFFECT__
#define __CY_EFFECT__

#include "cyclopsConfig.h"
#include "Material.h"

#include <osgFX/Effect>
#include <osgFX/Technique>
#include <osg/Material>
#include <osg/StateSet>

#define OMEGA_NO_GL_HEADERS
#include <omega.h>
#include <omegaOsg.h>
#include <omegaToolkit.h>

namespace cyclops {
	using namespace omega;
	using namespace omegaOsg;
	class SceneManager;

	///////////////////////////////////////////////////////////////////////////////////////////////
	class CY_API EffectNode: public osgFX::Effect
	{
	public:

	public:
		EffectNode() {}
		EffectNode(SceneManager* sm);
		virtual ~EffectNode();

		EffectNode(const EffectNode& copy, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY) : osgFX::Effect(copy, op) {	}

        // Effect class info
        META_Effect(cyclops, EffectNode, "EffectNode",
                    "Base effect class for cyclops effects.",
                    "Alessandro Febretti");	

		virtual bool define_techniques();

		String getDefinition() { return myDefinition; }
		void setDefinition(const String& definition);

		SceneManager* getSceneManager() { return mySceneManager; }

		Material* getMaterial(unsigned int index);
		void addMaterial(Material* mat);
		int getMaterialCount();
		void clearMaterials();

	private:
		Ref<osgFX::Technique> myCurrentTechnique;

		Vector< Ref<Material> > myMaterials;

		String myDefinition;
		SceneManager* mySceneManager;
	};
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	inline int EffectNode::getMaterialCount()
	{ return myMaterials.size(); }
};

#endif
