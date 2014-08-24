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
#include "cyclops/LightingLayer.h"

// Bullet and osgBullet
#include <btBulletDynamicsCommon.h>

using namespace cyclops;
using namespace omegaToolkit;
using namespace omegaToolkit::ui;

void cyclopsPythonApiInit();

SceneManager* SceneManager::mysInstance = NULL;

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
// SceneManagerWrapper is needed to SceneManager does not derive from 
// EngineModule and ShaderManager both. This class simply forwards engine module 
// methods to SceneManager
class SceneManagerWrapper: public EngineModule
{
public:
    SceneManagerWrapper(SceneManager* sm):
        EngineModule("SceneManager"),
        mySceneManager(sm)
    {}

    virtual void initialize() 
    { mySceneManager->initialize(); }

    virtual void dispose() 
    { mySceneManager->dispose(); }

    virtual void update(const UpdateContext& context)
    { mySceneManager->update(context); }

    virtual void handleEvent(const Event& evt)
    { mySceneManager->handleEvent(evt); }

    virtual bool handleCommand(const String& cmd)
    { return mySceneManager->handleCommand(cmd); }

private:
    Ref<SceneManager> mySceneManager;
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
        SceneManagerWrapper* smw = new SceneManagerWrapper(mysInstance);
        ModuleServices::addModule(smw);
        smw->doInitialize(Engine::instance());
    }
    return mysInstance;
}

///////////////////////////////////////////////////////////////////////////////
SceneManager::SceneManager():
    myOsg(NULL),
    mySkyBox(NULL),
    myWandTracker(NULL),
    myWandEntity(NULL),
    myDynamicsWorld(NULL),
    myPhysicsEnabled(false),
    myColDetectionEnabled(false),
    myEngine(Engine::instance())
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

    if(myDynamicsWorld != NULL)
    {
        delete myDynamicsWorld;
        myDynamicsWorld = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
LightingLayer* SceneManager::getLightingLayer() 
{ 
    return myLightingLayer; 
}

///////////////////////////////////////////////////////////////////////////////
CompositingLayer* SceneManager::getCompositingLayer()
{
    return myCompositingLayer;
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::loadConfiguration()
{
    Config* cfg = SystemManager::instance()->getAppConfig();
    Setting& s = cfg->lookup("config");

    // Set the default texture and attach it to the scene root.
    String defaultTextureName = "cyclops/common/defaultTexture.png";
    osg::Texture2D* defaultTexture = getTexture(defaultTextureName);
    if(defaultTexture != NULL)
    {
        osg::StateSet* ss = myCompositingLayer->getOsgNode()->getOrCreateStateSet();
        ss->setTextureAttribute(0, defaultTexture);
    }
    else
    {
        ofwarn("Could not load default texture %1%", %defaultTexture);
    }
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::initialize()
{
    // Make sure the osg module is initialized.
    if(!myOsg->isInitialized()) myOsg->initialize();

    // Pass myself to the lighting layer, so it will use me as the shader manager.
    myLightingLayer = new LightingLayer(this);
    myCompositingLayer = new CompositingLayer();

    myCompositingLayer->addLayer(myLightingLayer);

    myOsg->setRootNode(myCompositingLayer->getOsgNode());
    //myOsg->setRootNode(myRootLayer->getOsgNode());


    loadConfiguration();

    myModelLoaderThread = new ModelLoaderThread(this);
    myModelLoaderThread->start();

    // Set the menu manager
    myMenuManager = omegaToolkit::ui::MenuManager::instance();

    initDynamicsWorld();
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::initDynamicsWorld()
{
    btDefaultCollisionConfiguration * collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher * dispatcher = new btCollisionDispatcher( collisionConfiguration );
    btBroadphaseInterface * inter = new btDbvtBroadphase();
    btConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

    myDynamicsWorld = new btDiscreteDynamicsWorld( dispatcher, inter, solver, collisionConfiguration );
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

    ofmsg("SceneManager::unload: releasing %1% programs", %myPrograms.size());
    myPrograms.clear();

    ofmsg("SceneManager::unload: releasing %1% programs", %myTextures.size());
    myTextures.clear();
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::update(const UpdateContext& context) 
{
    // Update the scene layers.
    myCompositingLayer->update();

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

    // Update physics simulation
    if(myPhysicsEnabled)
    {
        myDynamicsWorld->stepSimulation( context.dt, 4, context.dt/2. );
    }
    if (myColDetectionEnabled)
    {
        myDynamicsWorld->performDiscreteCollisionDetection();
    }
}

///////////////////////////////////////////////////////////////////////////////
Uniforms* SceneManager::getGlobalUniforms() 
{ 
    if(myGlobalUniforms == NULL)
    {
        osg::StateSet* ss = myCompositingLayer->getOsgNode()->getOrCreateStateSet();
        myGlobalUniforms = new Uniforms(ss);
    }
    return myGlobalUniforms; 
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::handleEvent(const Event& evt) 
{
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::load(SceneLoader* loader)
{
    loader->startLoading(this);
    while(!loader->isLoadingComplete()) loader->loadStep();
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
void SceneManager::setBackgroundColor(const Color& color)
{
    myEngine->getDisplaySystem()->setBackgroundColor(color);
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
    static Lock smodloaderlock;
    smodloaderlock.lock();

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
#ifdef omegaOsgEarth_ENABLED
            if( info->mapName != "" ) {
                ModelAsset *mapAsset = getModel(info->mapName);
                DefaultModelLoader* mdl = dynamic_cast<DefaultModelLoader*>(myDefaultLoader);
                if(mdl) result = mdl->load(asset, mapAsset);
                else result = myDefaultLoader->load(asset);
            } else {
                result = myDefaultLoader->load(asset);
            }
#else
            result = myDefaultLoader->load(asset);
#endif
        }
    }
    omsg("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< SceneManager::loadModel\n");
    smodloaderlock.unlock();
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
        myLightingLayer->getOsgNode()->removeChild(mySkyBox->getNode());
    }

    mySkyBox = skyBox;
    if(mySkyBox != NULL)
    {
        setShaderMacroToFile("vsinclude envMap", "cyclops/common/envMap/cubeEnvMap.vert");
        setShaderMacroToFile("fsinclude envMap", "cyclops/common/envMap/cubeEnvMap.frag");
        omsg("Environment cube map shaders enabled");
        mySkyBox->initialize(myLightingLayer->getOsgNode()->getOrCreateStateSet());
        myLightingLayer->getOsgNode()->addChild(mySkyBox->getNode());
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

    //! If we lose tracking, (almost) immediately hide the wand.
    //! This works particularly well with the mvi WandPointerConverter service,
    //! Since it makes the wand hidden when the user is pointing at a 2D container. 
    myWandEntity->getTracker()->setAutoHideEnabled(true);
    myWandEntity->getTracker()->setHideTimeout(0.1f);

    myWandEntity->setFollowOffset(Vector3f(0, 0, 0), Quaternion(AngleAxis(Math::Pi, Vector3f::UnitY())));
    myEngine->getDefaultCamera()->addChild(myWandEntity);
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::hideWand(uint wandId)
{
    if(myWandEntity != NULL)
    {
        myWandEntity->setVisible(false);
        myWandEntity->unfollow();
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

///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
omegaToolkit::ui::Menu* SceneManager::createContextMenu(Entity* entity)
{
    myEntitiesWithMenu.push_back(entity);
    return myMenuManager->createMenu(entity->getName());
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::deleteContextMenu(Entity* entity)
{
    myEntitiesWithMenu.remove(entity);
}

///////////////////////////////////////////////////////////////////////////////
void SceneManager::setGravity(const Vector3f& g)
{
    oassert(myDynamicsWorld != NULL);
    myDynamicsWorld->setGravity(btVector3(g[0], g[1], g[2]));
}

///////////////////////////////////////////////////////////////////////////////
Vector3f SceneManager::getGravity()
{
    oassert(myDynamicsWorld != NULL);
    const btVector3& bg = myDynamicsWorld->getGravity();
    return Vector3f(bg[0], bg[1], bg[2]);
}
