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
 *-------------------------------------------------------------------------------------------------
 *	cyhello
 *		A basic omegalib/cyclops application. It shows how to use the advanced omegalib + OpenSceneGraph 
 *		integration library (cyclops) To create a simple scene made of a few basic primitives.
 **************************************************************************************************/
#include <omega.h>
#include <cyclops/cyclops.h>

using namespace omega;
using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////////////////////////
class HelloApplication: public EngineModule
{
public:
	HelloApplication();
	virtual void initialize();

private:
	SceneManager* mySceneManager;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
HelloApplication::HelloApplication():
	EngineModule("HelloApplication"),
	mySceneManager(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HelloApplication::initialize()
{
	// Create and initialize the cyclops scene manager.
	mySceneManager = SceneManager::createAndInitialize();

	SphereShape* sphere = new SphereShape(mySceneManager, 0.1f);
	sphere->setEffect("colored -d red");
	sphere->setPosition(0, 1.57, -3.24);

	PlaneShape* plane = new PlaneShape(mySceneManager, 4, 4);
	plane->setEffect("colored -d green");
	plane->pitch(-90 * Math::DegToRad);
	plane->setPosition(0, 0, -5);

	// Setup a light for the scene.
	Light* light = new Light(mySceneManager);
	light->setEnabled(true);
	light->setPosition(Vector3f(0, 50, 0));
	light->setColor(Color(1.0f, 1.0f, 0.7f));
	light->setAmbient(Color(0.1f, 0.1f, 0.1f));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Application entry point
int main(int argc, char** argv)
{
	Application<HelloApplication> app("cyhello");
	return omain(app, argc, argv);
}

