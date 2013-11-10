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
#ifndef __CY_SCENE_MANAGER__
#define __CY_SCENE_MANAGER__

#include "cyclopsConfig.h"
#include "Skybox.h"
#include "Shapes.h"
#include "Uniforms.h"
#include "ModelLoader.h"
#include "ShaderManager.h"
#include "LightingLayer.h"
#include "CompositingLayer.h"
#include "Compositor.h"

#include <osg/Texture2D>
#include <osg/Light>
#include <osg/Group>
#include <osg/Switch>
#include <osgAnimation/BasicAnimationManager>

#define OMEGA_NO_GL_HEADERS
#include <omega.h>
#include <omegaOsg/omegaOsg.h>
#include <omegaToolkit.h>

// Forward declarations
class ModelLoaderThread;
class btDynamicsWorld;

namespace cyclops {
    using namespace omega;
    using namespace omegaOsg;

    class SceneLoader;
    class SceneManager;
    class AnimatedObject;
    class ModelGeometry;

    ///////////////////////////////////////////////////////////////////////////
    //! The scene manager contains all the main features used to handle a 
    //! cyclops scene and its assets.
    //! @remarks SceneManager derives from ShaderManager to keep API 
    //! compatibility with omegalib 4.x (shader management methods appeared in 
    //! SceneManager)
    class CY_API SceneManager: public ShaderManager
    {
    friend class Entity;
    friend class Light;
    public:
        typedef AsyncTask< std::pair< Ref<ModelInfo>, bool > > LoadModelAsyncTask;
        enum AssetType { ModelAssetType };

    public:
        //! Creates and initializes the scene manager singleton.
        //! If called multiple times, subsequent calls will do nothing.
        static SceneManager* createAndInitialize();
        //! Returns an instance of the SceneManager singleton instance If no
        //! Scene manager exists before this call, createAndInitialize will be called internally.
        static SceneManager* instance();

        CompositingLayer* getCompositingLayer();
        LightingLayer* getLightingLayer();

        void initialize();
        void dispose();
        void update(const UpdateContext& context);
        void handleEvent(const Event& evt);
        bool handleCommand(const String& cmd);
        Engine* getEngine() { return myEngine; }

        //! Sets the background color
        void setBackgroundColor(const Color& color);
        //! Returns the global uniforms object. Can be used to set uniforms that will apply to all entities.
        Uniforms* getGlobalUniforms();

        //! Model Management
        //@{
        bool loadModel(ModelInfo* info);
        LoadModelAsyncTask* loadModelAsync(ModelInfo* info);
        void loadModelAsync(ModelInfo* info, const String& callback);
        void addModel(ModelGeometry* geom);
        ModelAsset* getModel(const String& name);
        const List< Ref<ModelAsset> >& getModels();
        void addLoader(ModelLoader* loader);
        void removeLoader(ModelLoader* loader);
        //@}

        //! Wand Management
        //@{
        void displayWand(uint wandId, uint trackableId);
        void hideWand(uint wandId);
        void setWandEffect(uint wandId, const String& effect);
        void setWandSize(float width, float length);
        //@}

        //! Scene creation methods
        //@{
        void load(SceneLoader* loader);
        //! Utility method: loads a scene file using the standard cyclops scene loader.
        void loadScene(const String& file);
        void setSkyBox(Skybox* skyBox);
        void unload();
        //@}

        osg::Texture2D* getTexture(const String& name);
        osg::Texture2D* createTexture(const String& name, PixelData* pixels);

        //! Physics support
        //@{
        void setGravity(const Vector3f& g);
        Vector3f getGravity();
        btDynamicsWorld* getDynamicsWorld() { return myDynamicsWorld; }
        void setPhysicsEnabled(bool value) { myPhysicsEnabled = value; }
        bool isPhysicsEnabled() { return myPhysicsEnabled; }
        //@}

        omegaToolkit::ui::Menu* createContextMenu(Entity* entity);
        void deleteContextMenu(Entity* entity);

        OsgModule* getOsgModule() { return myOsg; }

    private:
        SceneManager();
        virtual ~SceneManager();

        void initDynamicsWorld();
        void loadConfiguration();

    private:
        static SceneManager* mysInstance;
        Ref<Engine> myEngine;
        Ref<OsgModule> myOsg;

        // The scene global uniforms.
        Ref<Uniforms> myGlobalUniforms;

        // Model data (stored as dictionary and list for convenience)
        Dictionary<String, Ref<ModelAsset> > myModelDictionary;
        List< Ref<ModelAsset> > myModelList;
        ModelLoaderThread* myModelLoaderThread;
        // Model loaders
        Dictionary< String, Ref<ModelLoader> > myLoaderDictionary;
        // The default loader. Used when all the other loaders fail.
        ModelLoader* myDefaultLoader;
        
        Ref<CompositingLayer> myCompositingLayer;
        Ref<LightingLayer> myLightingLayer;

        Dictionary<String, Ref<osg::Texture2D> > myTextures;
        Dictionary<String, Ref<PixelData> > myTexturePixels;

        Ref<Skybox> mySkyBox;

        // Wand
        Ref<omega::TrackedObject> myWandTracker;
        Ref<CylinderShape> myWandEntity;

        // Context menu stuff.
        List< Entity* > myEntitiesWithMenu;
        Ref<omegaToolkit::ui::MenuManager> myMenuManager;

        // Physics stuff
        bool myPhysicsEnabled;
        btDynamicsWorld* myDynamicsWorld;
    };
};

#endif
