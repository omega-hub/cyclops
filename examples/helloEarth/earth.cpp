/********************************************************************************************************************** 
 * THE OMEGA LIB PROJECT
 *---------------------------------------------------------------------------------------------------------------------
 * Copyright 2010								Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti							febret@gmail.com
 *---------------------------------------------------------------------------------------------------------------------
 * Copyright (c) 2010, Electronic Visualization Laboratory, University of Illinois at Chicago
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
 * following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
 * disclaimer. Redistributions in binary form must reproduce the above copyright notice, this list of conditions 
 * and the following disclaimer in the documentation and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------------------------------------------------
 *	ohello
 *		Implements the simplest possible omegalib application.  Just a synchronized renderer that performs some 
 *		opengl drawing on one or multiple nodes. 
 *********************************************************************************************************************/
#include <omega.h>
#include <cyclops/cyclops.h>

using namespace omega;
using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EarthApplication: public EngineModule
{
public:
	EarthApplication(): EngineModule("EarthApplication") {}

    virtual void initialize()
    {
		ModelInfo* earth_model = new ModelInfo("earth", "mapquest_osm.earth",
			5.0f, 1, false, false, false, "", "mapquest_osm.earth");
		SceneManager::instance()->loadModel(earth_model);

		// Create a new object using the loaded model (referenced using its name, 'simpleModel')
		StaticObject* earth_object = new StaticObject(SceneManager::instance(), "earth");
		earth_object->setName("earth_object");
		earth_object->pitch(-90 * Math::DegToRad);
		earth_object->setPosition(0, 0, -2);

		Light* light = new Light(SceneManager::instance());
		light->setEnabled(true);
		light->setPosition(Vector3f(0, 50, 50));
		light->setColor(Color(1.0f, 1.0f, 1.0f));
		light->setAmbient(Color(0.1f, 0.1f, 0.1f));
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ApplicationBase entry point
int main(int argc, char** argv)
{
	Application<EarthApplication> app("earth");
    return omain(app, argc, argv);
}
