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
 * What's in this file:
 *  ModelGeometry lets users define their own custom geometry for drawing.
 ******************************************************************************/
#ifndef __CY_MODEL_GEOMETRY__
#define __CY_MODEL_GEOMETRY__

#include "cyclopsConfig.h"
#include "EffectNode.h"
#include "Uniforms.h"
#include "SceneManager.h"

#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>

#define OMEGA_NO_GL_HEADERS
#include <omega.h>
#include <omegaOsg/omegaOsg.h>
#include <omegaToolkit.h>

namespace cyclops {
	using namespace omega;
	using namespace omegaOsg;

	///////////////////////////////////////////////////////////////////////////
	class CY_API ModelGeometry: public ReferenceType
	{
	public:
		//! Creation method, to reflect the python API of most objects.
		static ModelGeometry* create(const String& name)
		{
			return new ModelGeometry(name);
		}

	public:
		ModelGeometry(const String& name);

		//! Adds a vertex and return its index.
		int addVertex(const Vector3f& v);
		//! Replaces an existing vertex
		void setVertex(int index, const Vector3f& v);
		//! Retrieves an existing vertex
		Vector3f getVertex(int index);
		//! Adds a vertex color and return its index. The color will be applied
		//! to the vertex with the same index as this color.
		int addColor(const Color& c);
		Color getColor(int index);
		//! Replaces an existing color
		void setColor(int index, const Color& c);
	

		//! Adds a primitive set
		void addPrimitive(ProgramAsset::PrimitiveType type, int startIndex, int endIndex);

		//! Removes all vertices, colors and primitives from this object
		void clear();

		const String& getName() { return myName; }
		osg::Geode* getOsgNode() { return myNode; }

	private:
		String myName;
		Ref<osg::Vec3Array> myVertices;
		Ref<osg::Vec4Array> myColors;
		Ref<osg::Geode> myNode;
		Ref<osg::Geometry> myGeometry;
	};
};

#endif
