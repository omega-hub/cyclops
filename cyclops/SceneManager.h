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
#include "Light.h"
#include "ModelLoader.h"

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
	struct ShadowSettings
	{
		bool shadowsEnabled;
		float shadowResolutionRatio;
	};

	///////////////////////////////////////////////////////////////////////////
	class ProgramAsset: public ReferenceType
	{
	public:
		enum PrimitiveType
		{
			Points = GL_POINTS,
			Triangles = GL_TRIANGLES,
			TriangleStrip = GL_TRIANGLE_STRIP
		};

	public:
		ProgramAsset():
			program(NULL), 
			vertexShaderBinary(NULL), 
			fragmentShaderBinary(NULL),
			geometryShaderBinary(NULL), 
			geometryOutVertices(0), 
			embedded(false)
		{}
	
		String name;
		String vertexShaderName;
		String fragmentShaderName;
		String geometryShaderName;

		bool embedded;
		String vertexShaderSource;
		String fragmentShaderSource;
		String geometryShaderSource;

		Ref<osg::Program> program;
		Ref<osg::Shader> vertexShaderBinary;
		Ref<osg::Shader> fragmentShaderBinary;
		Ref<osg::Shader> geometryShaderBinary;

		// Geometry shader parameters
		int geometryOutVertices;
		PrimitiveType geometryInput;
		PrimitiveType geometryOutput;
	};

	///////////////////////////////////////////////////////////////////////////
	//! The scene manager contains all the main features used to handle a 
	//! cyclops scene and its assets.
	class CY_API SceneManager: public EngineModule, public SceneNodeListener
	{
	friend class Entity;
	friend class Light;
	public:
		typedef AsyncTask< std::pair< Ref<ModelInfo>, bool > > LoadModelAsyncTask;
		typedef Dictionary<String, String> ShaderMacroDictionary;
		typedef Dictionary<String, String> ShaderCache;
		enum AssetType { ModelAssetType };

		static const int MaxLights = 16;

	public:
		static const int ReceivesShadowTraversalMask = 0x1;
		static const int CastsShadowTraversalMask = 0x2;

	public:
		//! Creates and initializes the scene manager singleton.
		//! If called multiple times, subsequent calls will do nothing.
		static SceneManager* createAndInitialize();
		//! Returns an instance of the SceneManager singleton instance If no
		//! Scene manager exists before this call, createAndInitialize will be called internally.
		static SceneManager* instance();

		virtual void initialize();
		virtual void dispose();
		virtual void update(const UpdateContext& context);
		virtual void handleEvent(const Event& evt);
		virtual bool handleCommand(const String& cmd);

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
		//! #PYAPI Utility method: loads a scene file using the standard cyclops scene loader.
		void loadScene(const String& file);
		void setSkyBox(Skybox* skyBox);
		void unload();
		//@}

		//! Light management methods
		//@{
		Light* getMainLight() { return myMainLight; }
		void setMainLight(Light* light) { myMainLight = light; }
		const ShadowSettings& getCurrentShadowSettings();
		void resetShadowSettings(const ShadowSettings& settings);
		int getNumActiveLights();
		//@}

		osg::Group* getOsgRoot() { return myScene; }
		osg::Texture2D* getTexture(const String& name);
		osg::Texture2D* createTexture(const String& name, PixelData* pixels);

		//! Shader management
		//@{
		ProgramAsset* getOrCreateProgram(const String& name, const String& vertexShaderName, const String& fragmentShaderName);
		void addProgram(ProgramAsset* program);
		void updateProgram(ProgramAsset* program);
		ProgramAsset* createProgramFromString(const String& name, const String& vertexShaderCode, const String& fragmentShaderCode);
		void initShading();
		void setShaderMacroToString(const String& macroName, const String& macroString);
		void setShaderMacroToFile(const String& macroName, const String& path);
		void reloadAndRecompileShaders();
		void recompileShaders(ProgramAsset* program, const String& variationName = "");
		//@}

		//! Physics support
		//@{
		void setGravity(const Vector3f& g);
		Vector3f getGravity();
		btDynamicsWorld* getDynamicsWorld() { return myDynamicsWorld; }
		void setPhysicsEnabled(bool value) { myPhysicsEnabled = value; }
		bool isPhysicsEnabled() { return myPhysicsEnabled; }
		//@}

		//! SceneNodeListener overrides
		virtual void onAttachedToScene(SceneNode* source);
		virtual void onDetachedFromScene(SceneNode* source);
		
		omegaToolkit::ui::Menu* createContextMenu(Entity* entity);
		void deleteContextMenu(Entity* entity);

	private:
		SceneManager();
		virtual ~SceneManager();

		void initDynamicsWorld();

		void addLight(Light* l);
		void removeLight(Light* l);
		void updateLights();
		void loadConfiguration();
		void loadShader(osg::Shader* shader, const String& name);
		void compileShader(osg::Shader* shader, const String& source);
		void recompileShaders();

	private:
		static SceneManager* mysInstance;
		Ref<OsgModule> myOsg;

		// The scene root. This may be linked directly to myRoot or have some intermediate nodes inbetween
		// (i.e. for shadow map management)
		Ref<osg::Group> myScene;
		// The scene global uniforms.
		Ref<Uniforms> myGlobalUniforms;

		// Model data (stored as dictionary and list for convenience)
		Dictionary<String, Ref<ModelAsset> > myModelDictionary;
		List< Ref<ModelAsset> > myModelList;
		ModelLoaderThread* myModelLoaderThread;

		Dictionary<String, Ref<osg::Texture2D> > myTextures;
		Dictionary<String, Ref<PixelData> > myTexturePixels;
		Dictionary<String, Ref<ProgramAsset> > myPrograms;
		Dictionary<String, Ref<osg::Shader> > myShaders;

		ShaderMacroDictionary myShaderMacros;
		ShaderCache myShaderCache;

		Ref<Skybox> mySkyBox;

		// Lights and shadows
		List< Ref<Light> > myLights;
		Vector<Light*> myActiveLights;
		Ref<Light> myMainLight;
		
		Ref<osgShadow::ShadowedScene> myShadowedScene;
		Ref<osgShadow::SoftShadowMap> mySoftShadowMap;
		ShadowSettings myShadowSettings;
		int myNumActiveLights;
		String myShaderVariationName;

		// Wand
		Ref<omega::TrackedObject> myWandTracker;

		Ref<CylinderShape> myWandEntity;

		// Context menu stuff.
		List< Entity* > myEntitiesWithMenu;
		Ref<omegaToolkit::ui::MenuManager> myMenuManager;

		// Model loaders
		Dictionary< String, Ref<ModelLoader> > myLoaderDictionary;
		// The default loader. Used when all the other loaders fail.
		ModelLoader* myDefaultLoader;

		// Physics stuff
		bool myPhysicsEnabled;
		btDynamicsWorld* myDynamicsWorld;
	};

	///////////////////////////////////////////////////////////////////////////
	inline int SceneManager::getNumActiveLights()
	{ return myNumActiveLights; }
};

#endif
