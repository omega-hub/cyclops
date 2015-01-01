/**************************************************************************************************
 * THE OMEGA LIB PROJECT
 *-------------------------------------------------------------------------------------------------
 * Copyright 2010-2015		Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-------------------------------------------------------------------------------------------------
 * Copyright (c) 2010-2015, Electronic Visualization Laboratory, University of Illinois at Chicago
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
#ifndef __CY_SCENE_LOADER__
#define __CY_SCENE_LOADER__

#include "cyclopsConfig.h"

#define OMEGA_NO_GL_HEADERS
#include <omega.h>
#include <omegaToolkit.h>
#include <omegaOsg/omegaOsg.h>

#include <osg/Group>

#include "SceneManager.h"
#include "Shapes.h"
#include "StaticObject.h"
#include "AnimatedObject.h"

namespace cyclops {
	using namespace omega;
	using namespace omegaOsg;
	using namespace omega::xml;

	///////////////////////////////////////////////////////////////////////////////////////////////
	//! Performs parsing of an xml .scene file and loads a scene.
	class CY_API SceneLoader
	{
	public:
		static Vector4f readVector4f(omega::xml::TiXmlElement* elem, const String& attributeName);
		static Vector3f readVector3f(omega::xml::TiXmlElement* elem, const String& attributeName);
		static Vector2f readVector2f(omega::xml::TiXmlElement* elem, const String& attributeName);
		static float readFloat(omega::xml::TiXmlElement* elem, const String& attributeName, float defaultValue = 0.0f);
		static int readInt(omega::xml::TiXmlElement* elem, const String& attributeName, int defaultValue = 0);
		static bool readBool(omega::xml::TiXmlElement* elem, const String& attributeName, bool defaultValue = false);

		//! #PYAPI @internal returns a pointer to the last entity loaded. Used for scripting support.
		static Entity* getLastLoadedEntity();

	public:
		SceneLoader(omega::xml::TiXmlDocument& doc, const String& path);

		//! Loader interface
		//@{
		void startLoading(SceneManager* sm);
		bool isLoadingComplete();
		int getLoadProgress();
		void loadStep();
		//@}

		void loadAssets(omega::xml::TiXmlElement* xStaticObjectFiles, SceneManager::AssetType type);

		void createObjects(osg::Group* root, omega::xml::TiXmlElement* xStaticObjectFiles);
		void createPrimitives(osg::Group* root, omega::xml::TiXmlElement* xStaticObjectFiles);

		void initShading();

	private:
		static Entity* sLastLoadedEntity;

		String myPath;
		omega::xml::TiXmlDocument& myDoc;
		SceneManager* mySceneManager;

	private:
		PlaneShape* createPlane(omega::xml::TiXmlElement* xPlane);
		SphereShape* createSphere(omega::xml::TiXmlElement* xchild);
		StaticObject* createStaticObject(omega::xml::TiXmlElement* xchild);
		AnimatedObject* createEntity(omega::xml::TiXmlElement* xchild);
	};
};

#endif