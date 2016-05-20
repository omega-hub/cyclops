/******************************************************************************
 * THE OMEGA LIB PROJECT
 *-----------------------------------------------------------------------------
 * Copyright 2010-2015		Electronic Visualization Laboratory, 
 *							University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-----------------------------------------------------------------------------
 * Copyright (c) 2010-2015, Electronic Visualization Laboratory,  
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
 * A manager that stores all programs and shaders that make p an independent 
 * shading environment. Multiple shading environments can exist at the same time
 * in a cyclops scene (for instance for sub-scenes with different light
 * settings)
 ******************************************************************************/
#ifndef __CY_SHADER_MANAGER__
#define __CY_SHADER_MANAGER__

#include <omegaOsg/omegaOsg.h>
#include <osg/Shader>
#include <osg/Program>

#include "cyclopsConfig.h"
#include "Light.h"

namespace cyclops {
	///////////////////////////////////////////////////////////////////////////
	//! Stores all programs and shaders that make p an independent shading
	//! environment. Multiple shading environments can exist at the same time
	//! in a cyclops scene (for instance for sub-scenes with different light
	//! settings)
	class ProgramAsset: public ReferenceType
	{
	public:
		enum PrimitiveType
		{
			Points = GL_POINTS,
			Triangles = GL_TRIANGLES,
			TriangleStrip = GL_TRIANGLE_STRIP,
			LineStrip = GL_LINE_STRIP
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

	class CY_API ShaderManager: public ReferenceType
	{
	public:
		typedef Dictionary<String, String> ShaderMacroDictionary;
		typedef Dictionary<String, String> ShaderCache;

		// A shader may process at most 8 simultaneous shadow maps.
		static const int MaxShadows = 4;
		// First texture unit used by shadow maps.
		static const int ShadowFirstTexUnit = 4;

	public:
		ShaderManager();
		~ShaderManager();

		void addLightInstance(LightInstance* l);
		void removeLightInstance(LightInstance* l);

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
		//! CacheId is used as a cache identifier to re-use results of previous 
		//! recompilations. In general, when some shader macro changes, a new 
		//! unique cacheId  should be used.
		void setActiveCacheId(const String& cacheId);
		const String& getActiveCacheId();

		void recompileShaders();
		void update();
		//@}

	private:
		void loadShader(osg::Shader* shader, const String& name);
		void compileShader(osg::Shader* shader, const String& source);

	protected:
		Dictionary<String, Ref<ProgramAsset> > myPrograms;
		Dictionary<String, Ref<osg::Shader> > myShaders;

		ShaderMacroDictionary myShaderMacros;
		ShaderCache myShaderCache;

		List<LightInstance*> myActiveLights;

		int myNumActiveLights;
		String myActiveCacheId;
		String myShaderVariationName;
	};
};


#endif
