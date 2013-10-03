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
 *	The scene manager contains all the main features used to handle a cyclops 
 *  scene and its assets.
 ******************************************************************************/
#include <osgUtil/Optimizer>
#include <osgDB/Archive>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgAnimation/Animation>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/TangentSpaceGenerator>

#include "cyclops/AnimatedObject.h"
#include "cyclops/SceneManager.h"
#include "cyclops/SceneLoader.h"
#include "cyclops/Shapes.h"
#include "cyclops/ModelGeometry.h"

using namespace cyclops;
using namespace omegaToolkit;
using namespace omegaToolkit::ui;

void cyclopsPythonApiInit();

SceneManager* SceneManager::mysInstance = NULL;

#ifdef OMEGA_OS_LINUX
	// On linux we need to define all static variables, even if they have been assigned in the header file.
	const int SceneManager::MaxLights;
#endif

Lock sModelQueueLock;
Queue< Ref<SceneManager::LoadModelAsyncTask> > sModelQueue;
bool sShutdownLoaderThread = false;

///////////////////////////////////////////////////////////////////////////////
class ModelLoaderThread: public Thread
{
public:
	ModelLoaderThread(SceneManager* mng): mySceneManager(mng)
	{}

	virtual void threadProc()
	{
		omsg("ModelLoaderThread: start");

		while(!sShutdownLoaderThread)
		{
			if(sModelQueue.size() > 0)
			{
				sModelQueueLock.lock();

				Ref<SceneManager::LoadModelAsyncTask> task = sModelQueue.front();
				sModelQueue.pop();

				bool res = mySceneManager->loadModel(task->getData().first);
				if(!sShutdownLoaderThread)
				{
					task->getData().second = res;
					task->notifyComplete();
				}

				sModelQueueLock.unlock();
			}
			osleep(100);
		}

		omsg("ModelLoaderThread: shutdown");
	}

private:
	SceneManager* mySceneManager;
};

///////////////////////////////////////////////////////////////////////////////
SceneManager* SceneManager::instance() 
{ 
	if(mysInstance == NULL)
	{
		createAndInitialize();
	}
	return mysInstance; 
}

///////////////////////////////////////////////////////////////////////////////
SceneManager* SceneManager::createAndInitialize()
{
	if(mysInstance == NULL)
	{
		mysInstance = new SceneManager();
		ModuleServices::addModule(mysInstance);
		mysInstance->doInitialize(Engine::instance());
	}
	return mysInstance;
}

///////////////////////////////////////////////////////////////////////////////
SceneManager::SceneManager():
	EngineModule("SceneManager"),
	myOsg(NULL),
	myShadowedScene(NULL),
	mySoftShadowMap(NULL),
	mySkyBox(NULL),
	myNumActiveLights(0),
	myWandTracker(NULL),
	myWandEntity(NULL)
{
#ifdef OMEGA_USE_PYTHON
	cyclopsPythonApiInit();
#endif

	myOsg = OsgModule::instance();

	myModelLoaderThread = NULL;
	sShutdownLoaderThread = false;

	myDefaultLoader = new DefaultModelLoader();

	// Add the default loader to the list of loaders, so it can be called explicitly. 
	// The default loader will always be used last.
	addLoader(myDefaultLoader);
}

///////////////////////////////////////////////////////////////////////////////
SceneManager::~SceneManager()
{
	mysInstance = NULL;

	if(myModelLoaderThread != NULL)
	{
		sShutdownLoaderThread = true;
		myModelLoaderThread->stop();
		delete myModelLoaderThread;
		myModelLoaderThread = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::loadConfiguration()
{
	Config* cfg = SystemManager::instance()->getAppConfig();
	Setting& s = cfg->lookup("config");

	String shadowMode = Config::getStringValue("shadowMode", s, "noshadows");
	StringUtils::toLowerCase(shadowMode);
	if(shadowMode == "noshadows") myShadowSettings.shadowsEnabled = false;
	else if(shadowMode == "softshadows") myShadowSettings.shadowsEnabled = true;

	myShadowSettings.shadowResolutionRatio = Config::getFloatValue("shadowResolutionRatio", s, 1.0f);

	omsg("SceneManager configuration loaded");
	ofmsg("::    Shadows enabled: %1%", %myShadowSettings.shadowsEnabled);
	ofmsg(":: Shadow resolution ratio: %1%", %myShadowSettings.shadowResolutionRatio);

	myScene = new osg::Group();

	// Set the default texture and attach it to the scene root.
	String defaultTextureName = "cyclops/common/defaultTexture.png";
	osg::Texture2D* defaultTexture = getTexture(defaultTextureName);
	if(defaultTexture != NULL)
	{
		myScene->getOrCreateStateSet()->setTextureAttribute(0, defaultTexture);
	}
	else
	{
		ofwarn("Could not load default texture %1%", %defaultTexture);
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::initialize()
{
	myMainLight = NULL;
	//myEngine = getEngine();

	// Make sure the osg module is initialized.
	if(!myOsg->isInitialized()) myOsg->initialize();

	loadConfiguration();


	// Standard shaders
#ifdef APPLE
	setShaderMacroToFile("surfaceShader", "cyclops/common/forward/default_osx.frag");
#else 
	setShaderMacroToFile("surfaceShader", "cyclops/common/forward/default.frag");
#endif
	setShaderMacroToFile("vertexShader", "cyclops/common/forward/default.vert");

	// Standard shaders
	setShaderMacroToFile("tangentSpaceSurfaceShader", "cyclops/common/forward/tangentSpace.frag");
	setShaderMacroToFile("tangentSpaceVertexShader", "cyclops/common/forward/tangentSpace.vert");
	//setShaderMacroToFile("tangentSpaceFragmentLightSection", "cyclops/common/forward/tangentSpaceLight.frag");

	setShaderMacroToFile("vsinclude envMap", "cyclops/common/envMap/noEnvMap.vert");
	setShaderMacroToFile("fsinclude envMap", "cyclops/common/envMap/noEnvMap.frag");

	setShaderMacroToFile("fsinclude lightFunctions", "cyclops/common/forward/lightFunctions.frag");

	setShaderMacroToString("customFragmentDefs", "");
	setShaderMacroToFile("postLightingSection", "cyclops/common/postLighting/default.frag");

	setShaderMacroToString("unlit", 
		"$@fragmentLightSection\n"
		"{\n" 
		"litSurfData.luminance = surf.albedo;\n"
		"}\n" 	
		"$\n");

	resetShadowSettings(myShadowSettings);

	myModelLoaderThread = new ModelLoaderThread(this);
	myModelLoaderThread->start();

	// Set the menu manager
	myMenuManager = omegaToolkit::ui::MenuManager::instance();
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::dispose()
{
	unload();

	// Release wand objects
	myWandEntity = NULL;
	myWandTracker = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::unload()
{
	sShutdownLoaderThread = true;
	myModelLoaderThread->stop();
	ofmsg("SceneManager::unload: emptying load queue (%1% queued items)", %sModelQueue.size());
	while(!sModelQueue.empty()) sModelQueue.pop();
	sShutdownLoaderThread = false;

	ofmsg("SceneManager::unload: releasing %1% models", %myModelList.size());
	myModelList.clear();
	myModelDictionary.clear();

	ofmsg("SceneManager::unload: releasing %1% lights", %myLights.size());
	myLights.clear();
	myMainLight = NULL;
	myNumActiveLights = 0;

	ofmsg("SceneManager::unload: releasing %1% programs", %myPrograms.size());
	myPrograms.clear();

	ofmsg("SceneManager::unload: releasing %1% programs", %myTextures.size());
	myTextures.clear();
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::update(const UpdateContext& context) 
{
	updateLights();

	// Loop through pixel buffers associated to textures. If a texture pixel buffer is dirty, 
	// update the relative texture.
	typedef pair<String, PixelData*> TexturePixelsItem;
	foreach(TexturePixelsItem item, myTexturePixels)
	{
		if(item.second->isDirty())
		{
			osg::Texture2D* texture = myTextures[item.first];
			osg::Image* img = OsgModule::pixelDataToOsg(item.second);
			texture->setImage(img);

			item.second->setDirty(false);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::onAttachedToScene(SceneNode* source)
{
	// Called by entities when their parent node changes. Update the osg parent node
	// accordingly.
	Entity* e = dynamic_cast<Entity*>(source);
	if(e != NULL)
	{
		myScene->addChild(e->getOsgNode());
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::onDetachedFromScene(SceneNode* source)
{
	// Called by entities when their parent node changes. Update the osg parent node
	// accordingly.
	Entity* e = dynamic_cast<Entity*>(source);
	if(e != NULL)
	{
		myScene->removeChild(e->getOsgNode());
	}
}

///////////////////////////////////////////////////////////////////////////////
Uniforms* SceneManager::getGlobalUniforms() 
{ 
	if(myGlobalUniforms == NULL)
	{
		myGlobalUniforms = new Uniforms(myScene->getOrCreateStateSet());
	}
	return myGlobalUniforms; 
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::addLight(Light* l)
{
	myLights.push_back(l);
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::removeLight(Light* l)
{
	myLights.remove(l);
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::updateLights()
{
	int i = 0;
	bool needShaderUpdate = false;
	foreach(Light* l, myLights)
	{
		if(l->isEnabled())
		{
			needShaderUpdate |= l->updateOsgLight(i++, myScene);
		}
	}

	// Setup shadow parameters for main light.
	if(myMainLight != NULL)
	{
		if(mySoftShadowMap != NULL)
		{
			mySoftShadowMap->setLight(myMainLight->myOsgLight);
			mySoftShadowMap->setSoftnessWidth(myMainLight->getSoftShadowWidth());
			mySoftShadowMap->setJitteringScale(myMainLight->getSoftShadowJitter());
		}
	}

	// If the number of lights changed, reset the shaders
	if(i != myNumActiveLights || needShaderUpdate)
	{
		ofmsg("Lights changed (active lights: %1%)", %i);

		// Set the number of lights shader macro parameter.
		myNumActiveLights = i;

		// Update active lights vector.
		myActiveLights.clear();
		foreach(Light* l, myLights)
		{
			if(l->isEnabled()) myActiveLights.push_back(l);
		}


		String numLightsString = ostr("%1%", %myNumActiveLights);
		setShaderMacroToString("numLights", numLightsString);
		recompileShaders();
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::handleEvent(const Event& evt) 
{
	if(evt.isButtonDown(getEngine()->getPrimaryButton()))
	{
		DisplaySystem* ds = SystemManager::instance()->getDisplaySystem();
		Ray r;
		bool res = ds->getViewRayFromEvent(evt, r);
		if(res)
		{
			Entity* menuEntity = NULL;
			float distance = 100000;
			foreach(Entity* e, myEntitiesWithMenu)
			{
				e->getContextMenu()->hide();
				Vector3f hitPoint;
				if(e->hit(r, &hitPoint, SceneNode::HitBest))
				{
					float d = (hitPoint - r.getOrigin()).norm();
					if(d < distance)
					{
						menuEntity = e;
						distance = d;
					}
				}
			}

			if(menuEntity != NULL)
			{
				Menu* m = menuEntity->getContextMenu();
				m->placeOnWand(evt);
				m->show();
				evt.setProcessed();
			}
			else
			{
				foreach(Entity* e, myEntitiesWithMenu)
				{
					e->getContextMenu()->hide();
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::load(SceneLoader* loader)
{
	loader->startLoading(this);
	while(!loader->isLoadingComplete()) loader->loadStep();
	if (myScene != NULL) 
	{
		osg::setNotifyLevel(osg::INFO);
		omsg("Optimizing scene graph...");
		// Optimize scenegraph
		osgUtil::Optimizer optOSGFile;
		//optOSGFile.optimize(node, osgUtil::Optimizer::ALL_OPTIMIZATIONS);
		osg::setNotifyLevel(osg::WARN);
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::loadScene(const String& relativePath)
{
	String fullPath;
	//If able to get full path
	if(DataManager::findFile( relativePath , fullPath))
	{
		//Stores the XML file to be parsed
		omega::xml::TiXmlDocument doc(fullPath.c_str());

		//Loads the XML file
		if(doc.LoadFile())
		{
			ofmsg("Loading scene: %1%...", %relativePath);

			//Instantiate a sceneLoader to load the entites in the XML file
			SceneLoader* sl = new SceneLoader(doc, fullPath);

			//	Gets the sceneManager if you do not have a pointer to the singleton sceneManager
			//Load the scene into the SceneManager via the SceneLoader
			load(sl);
		}
		else
		{
			//Error loading the XML
			ofwarn("sceneLoad Xml error at %1%:%2%.%3%: %4%", %relativePath %doc.ErrorRow() %doc.ErrorCol() %doc.ErrorDesc());
		}
	}
	else
	{
		//Error loacation the file
		ofwarn("!File not found: %1%", %relativePath);
	}
}

///////////////////////////////////////////////////////////////////////////////
osg::Texture2D* SceneManager::getTexture(const String& name)
{
	// If texture has been loaded already return it.
	if(myTextures.find(name) != myTextures.end())
	{
		return myTextures[name];
	}

	// Split the path and get the file name.
	String path;
	//String filename;
	//String extension;
	//StringUtils::splitFullFilename(name, filename, extension, path);

	if(DataManager::findFile(name, path))
	{
		//ofmsg("Loading texture file %1%", %filename);

        Ref<osg::Image> image;

		image = osgDB::readRefImageFile(path);

        if ( image != NULL )
        {
            osg::Texture2D* texture = new osg::Texture2D( image.get() );
            osg::Texture::WrapMode textureWrapMode;
            textureWrapMode = osg::Texture::REPEAT;

            texture->setWrap(osg::Texture2D::WRAP_R, textureWrapMode);
            texture->setWrap(osg::Texture2D::WRAP_S, textureWrapMode);
            texture->setWrap(osg::Texture2D::WRAP_T, textureWrapMode);

			myTextures[name] = texture;
			return texture;
        }
		else
		{
			ofwarn("Image not valid: %1%", %path);
		}
	}
	else
	{
		ofwarn("Could not find texture file %1%", %name);
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
osg::Texture2D* SceneManager::createTexture(const String& name, PixelData* pixels)
{
	osg::Texture2D* texture = new osg::Texture2D();
	osg::Image* img = OsgModule::pixelDataToOsg(pixels);
	texture->setImage(img);

	pixels->setDirty(false);
	myTexturePixels[name] = pixels;
	myTextures[name] = texture;

	return texture;
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::setShaderMacroToString(const String& macroName, const String& macroString)
{
	myShaderMacros[macroName] = macroString;
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::setShaderMacroToFile(const String& macroName, const String& name)
{
	String path;
	if(DataManager::findFile(name, path))
	{
		std::ifstream t(path.c_str());
		std::stringstream buffer;
		buffer << t.rdbuf();
		setShaderMacroToString(macroName, buffer.str());
	}
	else
	{
		ofwarn("SceneManager::setShaderMacroToFile: could not find file %1%", %name);
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::loadShader(osg::Shader* shader, const String& name)
{
	// If shader source is not in the cache, load it now.
	if(myShaderCache.find(name) == myShaderCache.end())
	{
		String path;
		if(DataManager::findFile(name, path))
		{
			ofmsg("Loading shader: %1%", %name);

			std::ifstream t(path.c_str());
			std::stringstream buffer;
			buffer << t.rdbuf();
			String shaderSrc = buffer.str();

			myShaderCache[name] = shaderSrc;
		}
		else
		{
			ofwarn("Could not find shader file %1%", %name);
		}
	}

	if(myShaderCache.find(name) != myShaderCache.end())
	{
		compileShader(shader, myShaderCache[name]);
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::compileShader(osg::Shader* shader, const String& source)
{
	String shaderPreSrc = source;
	String lightSectionMacroName = "fragmentLightSection";

	// Replace shader macros.
	// Do a multiple replacement passes to process macros-within macros.
	int replacementPasses = 3;
	for(int i = 0; i < replacementPasses; i++)
	{
		foreach(ShaderMacroDictionary::Item macro, myShaderMacros)
		{
			if(macro.getKey() != lightSectionMacroName)
			{
				String macroName = ostr("@%1%", %macro.getKey());
				shaderPreSrc = StringUtils::replaceAll(shaderPreSrc, macroName, macro.getValue());
			}
		}
	}

	// Read local macro definitions (only supported one now is fragmentLightSection)
	String shaderSrc = "";
	Vector<String> segments = StringUtils::split(shaderPreSrc, "$");
	//ofmsg("segments %1%", %segments.size());
	foreach(String segment, segments)
	{
		if(StringUtils::startsWith(segment, "@"))
		{
			Vector<String> macroNames = StringUtils::split(segment, "\r\n\t ", 1);
				
			String macroContent = StringUtils::replaceAll(segment, macroNames[0], "");
				
			String macroName = macroNames[0].substr(1);
			//ofmsg("SEGMENT IDENTIFIED: %1%", %macroName);
			myShaderMacros[macroName] = macroContent;
		}
		else
		{
			shaderSrc.append(segment);
		}
	}

	// Special section: replicate lighting code as many times as the active lights
	String fragmentShaderLightCode = myShaderMacros[lightSectionMacroName];
	String fragmentShaderLightSection = "";
	for(int i = 0; i < myNumActiveLights; i++)
	{
		Light* light = myActiveLights[i];

		// Add the light index to the section
		String fragmentShaderLightCodeIndexed = StringUtils::replaceAll(
			fragmentShaderLightCode, 
			"@lightIndex", 
			boost::lexical_cast<String>(i));

		// Replace light function call with light function name specified for light.
		fragmentShaderLightCodeIndexed = StringUtils::replaceAll(fragmentShaderLightCodeIndexed,
			"@lightFunction", light->getLightFunction());

		fragmentShaderLightSection += fragmentShaderLightCodeIndexed;
	}
	shaderSrc = StringUtils::replaceAll(shaderSrc, 
		"@" + lightSectionMacroName, 
		fragmentShaderLightSection);

	//ofmsg("Loading shader file %1%", %name);
	// omsg("#############################################################");
	// omsg(shaderSrc);
	// omsg("#############################################################");
	shader->setShaderSource(shaderSrc);
}

///////////////////////////////////////////////////////////////////////////////
ProgramAsset* SceneManager::getOrCreateProgram(const String& name, const String& vertexShaderName, const String& fragmentShaderName)
{
	// If program has been loaded already return it.
	if(myPrograms.find(name) != myPrograms.end())
	{
		return myPrograms[name];
	}

	ProgramAsset* asset = new ProgramAsset();
	asset->program = new osg::Program();
	asset->name = name;
	asset->program->setName(name);
	asset->fragmentShaderName = fragmentShaderName;
	asset->vertexShaderName = vertexShaderName;

	myPrograms[name] = asset;

	recompileShaders(asset, myShaderVariationName);

	return asset;
}

///////////////////////////////////////////////////////////////////////////////
ProgramAsset* SceneManager::createProgramFromString(const String& name, const String& vertexShaderCode, const String& fragmentShaderCode)
{
	ProgramAsset* asset = new ProgramAsset();
	asset->program = new osg::Program();
	asset->name = name;
	asset->program->setName(name);
	asset->fragmentShaderSource = fragmentShaderCode;
	asset->fragmentShaderName = name + "Fragment";
	asset->vertexShaderSource = vertexShaderCode;
	asset->vertexShaderName = name + "Vertex";
	asset->embedded = true;

	myPrograms[name] = asset;

	recompileShaders(asset, myShaderVariationName);

	return asset;
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::addProgram(ProgramAsset* program)
{
	myPrograms[program->name] = program;
	program->program = new osg::Program();
	program->program->setName(program->name);
	recompileShaders(program, myShaderVariationName);
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::updateProgram(ProgramAsset* program)
{
	// Delete current shaders to force reload.
	String fullVertexShaderName = program->vertexShaderName + myShaderVariationName;
	String fullFragmentShaderName = program->fragmentShaderName + myShaderVariationName;

	// Delete binaries...
	myShaders[fullVertexShaderName] = NULL;
	myShaders[fullFragmentShaderName] = NULL;

	// Delete source cache...
	myShaderCache.erase(program->vertexShaderName);
	myShaderCache.erase(program->fragmentShaderName);

	recompileShaders(program, myShaderVariationName);
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::recompileShaders(ProgramAsset* program, const String& svariationName)
{
	String var = myShaderVariationName;
	if(svariationName != "") var = svariationName;

	osg::Program* osgProg = program->program;

	// Remove current shaders from program
	osgProg->removeShader(program->vertexShaderBinary);
	osgProg->removeShader(program->fragmentShaderBinary);
	osgProg->removeShader(program->geometryShaderBinary);
	//osgProg->releaseGLObjects();

	String fullVertexShaderName = program->vertexShaderName + var;
	osg::Shader* vertexShader = myShaders[fullVertexShaderName];
	// If the shader does not exist in the shader registry, we need to create it now.
	if(vertexShader == NULL)
	{
		ofmsg("Creating vertex shader %1%", %fullVertexShaderName);

		vertexShader = new osg::Shader( osg::Shader::VERTEX );
		// increase reference count to avoid being deallocated by osg program when deattached.
		vertexShader->ref();
		
		// If the program asset has embedded code, use the code from the asset instead of looking up a file.
		if(program->embedded)
		{
			compileShader(vertexShader, program->vertexShaderSource);
		}
		else
		{
			loadShader(vertexShader, program->vertexShaderName);
		}
		myShaders[fullVertexShaderName] = vertexShader;
	}
	program->vertexShaderBinary = vertexShader;
	osgProg->addShader(vertexShader);

	String fullFragmentShaderName = program->fragmentShaderName + var;
	osg::Shader* fragmentShader = myShaders[fullFragmentShaderName];
	// If the shader does not exist in the shader registry, we need to create it now.
	if(fragmentShader == NULL)
	{
		ofmsg("Creating fragment shader %1%", %fullFragmentShaderName);

		fragmentShader = new osg::Shader( osg::Shader::FRAGMENT );
		// increase reference count to avoid being deallocated by osg program when deattached.
		fragmentShader->ref();
		// If the program asset has embedded code, use the code from the asset instead of looking up a file.
		if(program->embedded)
		{
			compileShader(fragmentShader, program->fragmentShaderSource);
		}
		else
		{
			loadShader(fragmentShader, program->fragmentShaderName);
		}
		myShaders[fullFragmentShaderName] = fragmentShader;
	}
	program->fragmentShaderBinary = fragmentShader;
	osgProg->addShader(fragmentShader);

	// OPTIONAL geometry shader
	if(program->geometryShaderName != "")
	{
		String fullGeometryShaderName = program->geometryShaderName + var;
		osg::Shader* geometryShader = myShaders[fullGeometryShaderName];
		// If the shader does not exist in the shader registry, we need to create it now.
		if(geometryShader == NULL)
		{
			ofmsg("Creating geometry shader %1%", %fullGeometryShaderName);

			geometryShader = new osg::Shader( osg::Shader::GEOMETRY );
			// increase reference count to avoid being deallocated by osg program when deattached.
			geometryShader->ref();
			// If the program asset has embedded code, use the code from the asset instead of looking up a file.
			if(program->embedded)
			{
				compileShader(geometryShader, program->geometryShaderSource);
			}
			else
			{
				loadShader(geometryShader, program->geometryShaderName);
			}
			myShaders[fullGeometryShaderName] = geometryShader;
		}
		program->geometryShaderBinary = geometryShader;
		osgProg->addShader(geometryShader);
		// Set geometry shader parameters.
		osgProg->setParameter( GL_GEOMETRY_VERTICES_OUT_EXT, program->geometryOutVertices );
		osgProg->setParameter( GL_GEOMETRY_INPUT_TYPE_EXT, program->geometryInput );
		osgProg->setParameter( GL_GEOMETRY_OUTPUT_TYPE_EXT, program->geometryOutput );
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::setBackgroundColor(const Color& color)
{
	getEngine()->getDisplaySystem()->setBackgroundColor(color);
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::addModel(ModelGeometry* geom)
{
	oassert(geom != NULL);
	ModelAsset* asset = new ModelAsset();
	asset->name = geom->getName();
	asset->numNodes = 1;
	asset->info = NULL;
	asset->nodes.push_back(geom->getOsgNode());

	myModelDictionary[asset->name] = asset;
	myModelList.push_back(asset);
}

///////////////////////////////////////////////////////////////////////////////
SceneManager::LoadModelAsyncTask* SceneManager::loadModelAsync(ModelInfo* info)
{
	sModelQueueLock.lock();
	LoadModelAsyncTask* task = new LoadModelAsyncTask();
	task->setData( LoadModelAsyncTask::Data(info, true) );
	sModelQueue.push(task);
	sModelQueueLock.unlock();
	return task;
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::loadModelAsync(ModelInfo* info, const String& callback)
{
	LoadModelAsyncTask* task = loadModelAsync(info);
	task->setCompletionCommand(callback);
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::addLoader(ModelLoader* loader)
{
	if(loader != NULL)
	{
		myLoaderDictionary[loader->getName()] = loader;
	}
	else
	{
		owarn("SceneManager::addLoader: can't add NULL loader");
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::removeLoader(ModelLoader* loader)
{
	if(loader != NULL)
	{
		myLoaderDictionary.erase(loader->getName());
	}
	else
	{
		owarn("SceneManager::removeLoader: can't remove NULL loader");
	}
}

///////////////////////////////////////////////////////////////////////////////
bool SceneManager::loadModel(ModelInfo* info)
{
	omsg(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SceneManager::loadModel");
	bool result = false;

	ModelAsset* asset = new ModelAsset();
	asset->name = info->path; /// changed filepath to filename (confirm from alassandro).
	asset->numNodes = info->numFiles;
	asset->info = info;

	myModelDictionary[info->name] = asset;
	myModelList.push_back(asset);

	Vector<String> args = StringUtils::tokenise(asset->name, " ", "'");
	// If we have 2 arguments, the first one is the name of the loader
	if(args.size() == 2)
	{
		if(myLoaderDictionary.find(args[0]) != myLoaderDictionary.end())
		{
			ModelLoader* loader = myLoaderDictionary[args[0]];
			asset->name = args[1];
			result = loader->load(asset);
		}
	}
	// A single argument is just a filename, find the loader by supported extension.
	else
	{
		String basename;
		String extension;
		StringUtils::splitBaseFilename(asset->name, basename, extension);
		typedef KeyValue<String, Ref<ModelLoader> > LoaderDictionaryItem;
		foreach(LoaderDictionaryItem ml, myLoaderDictionary)
		{
			if(ml.second != myDefaultLoader && ml->supportsExtension(extension))
			{
				if(ml->load(asset)) 
				{
					result = true;
					break;
				}
			}
		}
		// All loaders failed or none was able to handle the model file extension. Use the default loader.
		if(!result)
		{
			result = myDefaultLoader->load(asset);
		}
	}
	omsg("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< SceneManager::loadModel\n");
	return result;
}

///////////////////////////////////////////////////////////////////////////////
ModelAsset* SceneManager::getModel(const String& name)
{
	return myModelDictionary[name];
}

///////////////////////////////////////////////////////////////////////////////
const List< Ref<ModelAsset> >& SceneManager::getModels()
{
	return myModelList;
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::setSkyBox(Skybox* skyBox)
{
	// If a skybox is currently active, remove it.
	if(mySkyBox != NULL)
	{
		myScene->removeChild(mySkyBox->getNode());
	}

	mySkyBox = skyBox;
	if(mySkyBox != NULL)
	{
		setShaderMacroToFile("vsinclude envMap", "cyclops/common/envMap/cubeEnvMap.vert");
		setShaderMacroToFile("fsinclude envMap", "cyclops/common/envMap/cubeEnvMap.frag");
		omsg("Environment cube map shaders enabled");
		mySkyBox->initialize(myScene->getOrCreateStateSet());
		myScene->addChild(mySkyBox->getNode());
	}
	else
	{
		setShaderMacroToFile("vsinclude envMap", "cyclops/common/envMap/noEnvMap.vert");
		setShaderMacroToFile("fsinclude envMap", "cyclops/common/envMap/noEnvMap.frag");
		omsg("Environment cube map shaders disabled");
	}

	recompileShaders();
}

///////////////////////////////////////////////////////////////////////////////
const ShadowSettings& SceneManager::getCurrentShadowSettings()
{
	return myShadowSettings;
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::resetShadowSettings(const ShadowSettings& settings)
{
	myShadowSettings = settings;
	if(myShadowedScene == NULL)
	{
		myShadowedScene = new osgShadow::ShadowedScene();
		myShadowedScene->setReceivesShadowTraversalMask(SceneManager::ReceivesShadowTraversalMask);
		myShadowedScene->setCastsShadowTraversalMask(SceneManager::CastsShadowTraversalMask);
		myShadowedScene->addChild(myScene);
	}
	if(myShadowSettings.shadowsEnabled)
	{
		// compute the shadow map resolution. 
		int smHeight = 512;
		int smWidth = 512;

		ofmsg("SceneManager::resetShadowSettings: Shadow map size = (%1%x%2%)", %smWidth %smHeight);

		if(mySoftShadowMap == NULL)
		{
			mySoftShadowMap = new osgShadow::SoftShadowMap;
			// Hardcoded ambient bias for shadow map. Shadowed areas receive zero light. 
			// Unshadowed areas receive full light.
			mySoftShadowMap->setAmbientBias(osg::Vec2(0.0f, 1.0f));
			// Hardcoded texture unit arguments for shadow map.
			mySoftShadowMap->setTextureUnit(4);
			mySoftShadowMap->setJitterTextureUnit(5);
			myShadowedScene->setShadowTechnique(mySoftShadowMap);
		}
		mySoftShadowMap->setTextureSize(osg::Vec2s(smWidth, smHeight));

		setShaderMacroToFile("vsinclude shadowMap", "cyclops/common/shadowMap/softShadowMap.vert");
		setShaderMacroToFile("fsinclude shadowMap", "cyclops/common/shadowMap/softShadowMap.frag");

		myOsg->setRootNode(myShadowedScene);
	}
	else
	{
		myOsg->setRootNode(myScene);
		setShaderMacroToFile("vsinclude shadowMap", "cyclops/common/shadowMap/noShadowMap.vert");
		setShaderMacroToFile("fsinclude shadowMap", "cyclops/common/shadowMap/noShadowMap.frag");
	}

	recompileShaders();
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::recompileShaders()
{
	// Add light functions to shader variation name
	String lightFunc = "";
	foreach(Light* l, myActiveLights)
	{
		lightFunc.append(l->getLightFunction());
	}
#ifdef OMEGA_OS_WIN
	std::hash<String> hashFx;
#else
	std::tr1::hash<String> hashFx;
#endif
	size_t lightFuncHash = hashFx(lightFunc);

	// Update the shader variation name
	myShaderVariationName = ostr(myShadowSettings.shadowsEnabled ? ".sm%1%-%2$x" : ".%1%-%2$x", %myNumActiveLights %lightFuncHash);

	ofmsg("Recompiling shaders (variation: %1%)", %myShaderVariationName);

	typedef Dictionary<String, Ref<ProgramAsset> >::Item ProgramAssetItem;
	foreach(ProgramAssetItem item, myPrograms)
	{
		recompileShaders(item.getValue(), myShaderVariationName);
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::reloadAndRecompileShaders()
{
	myShaderCache.clear();
	myShaders.clear();
	recompileShaders();
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::displayWand(uint wandId, uint trackableId)
{
	// Simple single-wand implementation: we ignore wandId
	if(myWandEntity == NULL)
	{
		myWandEntity = new CylinderShape(this, 1, 1, 0.1);
		myWandEntity->setName("Wand");
		setWandSize(0.01f, 2);
		myWandEntity->setEffect("colored -d white");
	}
	myWandEntity->setVisible(true);
	myWandEntity->followTrackable(trackableId);
	myWandEntity->setFollowOffset(Vector3f(0, 0, 0), Quaternion(AngleAxis(Math::Pi, Vector3f::UnitY())));
	getEngine()->getDefaultCamera()->addChild(myWandEntity);
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::hideWand(uint wandId)
{
	if(myWandEntity != NULL)
	{
		myWandEntity->setVisible(false);
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::setWandEffect(uint wandId, const String& effect)
{
	// Simple single-wand implementation: we ignore wandId
	if(myWandEntity != NULL)
	{
		myWandEntity->setEffect(effect);
	}
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::setWandSize(float width, float length)
{
	// Simple single-wand implementation: we ignore wandId
	if(myWandEntity != NULL)
	{
		myWandEntity->setScale(width, width, length);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
bool SceneManager::handleCommand(const String& cmd)
{
	Vector<String> args = StringUtils::split(cmd);
	if(args[0] == "?" && args.size() == 1)
	{
		omsg("SceneManager");
		omsg("\t shaderInfo  - prints list of cached shaders");
	}
	else if(args[0] == "shaderInfo")
	{
		typedef Dictionary<String, Ref<osg::Shader> >::Item ShaderItem;
		foreach(ShaderItem si, myShaders)
		{
			omsg(si.getKey());
		}
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
omegaToolkit::ui::Menu* SceneManager::createContextMenu(Entity* entity)
{
	myEntitiesWithMenu.push_back(entity);
	return myMenuManager->createMenu(entity->getName());
}

///////////////////////////////////////////////////////////////////////////////////////////////
void SceneManager::deleteContextMenu(Entity* entity)
{
	myEntitiesWithMenu.remove(entity);
}

