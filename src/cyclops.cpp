/**************************************************************************************************
 * THE OMEGA LIB PROJECT
 *-------------------------------------------------------------------------------------------------
 * Copyright 2010-2013		Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-------------------------------------------------------------------------------------------------
 * Copyright (c) 2010-2011, Electronic Visualization Laboratory, University of Illinois at Chicago
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
#define OMEGA_NO_GL_HEADERS
#include <omega.h>
#include <omegaToolkit.h>

#include "SceneManager.h"
#include "SceneLoader.h"

using namespace omega;
using namespace omegaToolkit;
using namespace omegaOsg;
using namespace osg;
using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Cyclops: public ServerModule
{
public:
	Cyclops(): mySceneManager(NULL) {}
	virtual void initialize();
	virtual void update(const UpdateContext& context);
	virtual void handleEvent(const Event& evt);

private:
	SceneManager* mySceneManager;

	Vector3f myCenter;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
void Cyclops::initialize()
{
	Config* cfg = getEngine()->getSystemManager()->getAppConfig();

	getEngine()->removeAllRenderPasses();

	mySceneManager = new SceneManager();
	mySceneManager->initialize(getEngine());

	myCenter = Vector3f::Zero();

	// Add a new data path.
	if(cfg->exists("config/dataPath"))
	{
		String dataPath = String((const char*)cfg->lookup("config/dataPath"));
		DataManager* dm = SystemManager::instance()->getDataManager();
		dm->addSource(new FilesystemDataSource(dataPath));
	}

	// Load osg object
	if(cfg->exists("config/scene"))
	{
		String mapName = String((const char*)cfg->lookup("config/scene"));

		DataManager* dm = SystemManager::instance()->getDataManager();
		DataInfo cfgInfo = dm->getInfo(mapName);
		if(!cfgInfo.isNull())
		{
			TiXmlDocument doc(cfgInfo.path.c_str());
			if(doc.LoadFile())
			{
				ofmsg("Loading map: %1%...", %mapName);

				SceneLoader* sl = new SceneLoader(doc);
				mySceneManager->load(sl);
			}
		}
		else
		{
			ofwarn("!File not found: %1%", %mapName);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Cyclops::update(const UpdateContext& context) 
{
	mySceneManager->update(context);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Cyclops::handleEvent(const Event& evt) 
{
	mySceneManager->handleEvent(evt);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Application entry point
int main(int argc, char** argv)
{
	OmegaToolkitApplication<Cyclops> app("cyclops");
	return omain(app, argc, argv);
}
