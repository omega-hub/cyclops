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
 *-------------------------------------------------------------------------------------------------
 *	cyhello2
 *		A slightly more advanced cyclops application. Loads a mesh and lets the user manipulate it.
 *		This demo introduces the use of the SceneEditor classes and the menu system.
 **************************************************************************************************/
#include <omega.h>
#include <cyclops/cyclops.h>
#include <omegaToolkit.h>

using namespace omega;
using namespace cyclops;
using namespace omegaToolkit;
using namespace omegaToolkit::ui;

///////////////////////////////////////////////////////////////////////////////////////////////////
class HelloApplication: public EngineModule, IMenuItemListener, SceneNodeListener
{
public:
	HelloApplication();

	virtual void initialize();
	virtual void update(const UpdateContext& context);
	virtual void onMenuItemEvent(MenuItem* mi);
	virtual void onSelectedChanged(SceneNode* source, bool value);

private:
	SceneManager* mySceneManager;
	StaticObject* myObject;

	// Scene editor. This will be used to manipulate the object.
	SceneEditorModule* myEditor;

	// Menu stuff.
	MenuManager* myMenuManager;
	MenuItem* myQuitMenuItem;
	MenuItem* myEditorMenuItem;
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

	// Load some basic model from the omegalib meshes folder.
	// Force the size of the object to be 0.8 meters
	//ModelInfo torusModel("simpleModel", "demos/showcase/data/walker/walker.fbx", 2.0f);
	ModelInfo* torusModel = new ModelInfo("simpleModel", "cyclops/test/torus.fbx", 2.0f);
	mySceneManager->loadModel(torusModel);

	// Create a new object using the loaded model (referenced using its name, 'simpleModel')
	myObject = new StaticObject(mySceneManager, "simpleModel");
	myObject->setName("object");
	myObject->setEffect("colored -d #303030 -g 1.0 -s 20.0 -v envmap");
	// Add a selection listener to the object. HelloApplication::onSelectedChanged will be
	// called whenever the object selection state changes.
	myObject->pitch(-90 * Math::DegToRad);
	myObject->setPosition(0, 0, -2);
	myObject->addListener(this);

	// Create the scene editor and add our loaded object to it.
	myEditor = SceneEditorModule::createAndInitialize();
	myEditor->addNode(myObject);
	myEditor->setEnabled(false);

	// Create a plane for reference.
	PlaneShape* plane = new PlaneShape(mySceneManager, 100, 100, Vector2f(50, 50));
	plane->setName("ground");
	//plane->setEffect("textured -d cyclops/test/checker2.jpg -s 20 -g 1 -v envmap");
	plane->setEffect("textured -d cubemaps/gradient1/negy.png");
	plane->pitch(-90 * Math::DegToRad);
	plane->setPosition(0, 0, -2);

	// Setup a light for the scene.
	Light* light = new Light(mySceneManager);
	light->setEnabled(true);
	light->setPosition(Vector3f(0, 5, -2));
	light->setColor(Color(1.0f, 1.0f, 1.0f));
	light->setAmbient(Color(0.1f, 0.1f, 0.1f));
	//light->setSoftShadowWidth(0.01f);

	// Setup a light for the scene.
	Light* light2 = new Light(mySceneManager);
	light2->setEnabled(true);
	light2->setPosition(Vector3f(5, 0, -2));
	light2->setColor(Color(0.0f, 1.0f, 0.0f));

	// Setup a light for the scene.
	Light* light3 = new Light(mySceneManager);
	light3->setEnabled(true);
	light3->setPosition(Vector3f(0, 0, 3));
	light3->setColor(Color(0.0f, 0.0f, 1.0f));

	// Create and initialize the menu manager
	myMenuManager = MenuManager::createAndInitialize();

	// Create the root menu
	ui::Menu* menu = myMenuManager->createMenu("menu");
	myMenuManager->setMainMenu(menu);

	// Add the 'manipulate object' menu item. This item will be a checkbox. We attach this class as
	// a listener for the menu item: When the item is activated, the menu system will call
	// HelloApplication::onMenuItemEvent
	myEditorMenuItem = menu->addItem(MenuItem::Checkbox);
	myEditorMenuItem->setText("Manipulate object");
	myEditorMenuItem->setChecked(false);
	myEditorMenuItem->setListener(this);

	// Add the 'quit' menu item. Menu items can either run callbacks when activated, or they can 
	// run script commands. In this case, we make the menu item call the 'oexit' script command. 
	// oexit is the standard omegalib script command used to terminate the application.
	// Using scripting with menu items can be extremely powerful and flexible, and limits the amount
	// of callback code that needs to be written to handle ui events.
	myQuitMenuItem = menu->addItem(MenuItem::Button);
	myQuitMenuItem->setText("Quit");
	myQuitMenuItem->setCommand("oexit()");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HelloApplication::update(const UpdateContext& context)
{
	// If we are not in editing mode, Animate our loaded object.
	if(!myEditor->isEnabled())
	{
		myObject->setPosition(0, 2 + Math::sin(context.time) * 0.5f + 0.5f, -2);
		myObject->pitch(context.dt);
		myObject->yaw(context.dt);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HelloApplication::onMenuItemEvent(MenuItem* mi)
{
	// is this event generated by the 'manipulate object' menu item?
	if(mi == myEditorMenuItem)
	{
		myEditor->setEnabled(myEditorMenuItem->isChecked());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HelloApplication::onSelectedChanged(SceneNode* source, bool value)
{
	if(source == myObject)
	{
		if(myObject->isSelected())
		{
			// Color the object yellow when selected.
			myObject->setEffect("colored -d #ffff30 -g 1.0 -s 20.0");
		}
		else
		{
			// Color the object gray when not selected.
			myObject->setEffect("colored -d #303030 -g 1.0 -s 20.0");
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Application entry point
int main(int argc, char** argv)
{
	Application<HelloApplication> app("cyhello2");
	return omain(app, argc, argv);
}

