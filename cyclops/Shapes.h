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
 *************************************************************************************************/
#ifndef __CY_SHAPES__
#define __CY_SHAPES__

#include "Entity.h"

namespace cyclops {
	using namespace omega;
	using namespace omegaOsg;

	///////////////////////////////////////////////////////////////////////////////////////////////
	class CY_API CylinderShape: public Entity
	{
	public:
		//! Convenience method for creating SphereShape
		static CylinderShape* create(float length, float radius1, float radius2, int subdivisions, int sides);

	public:
		CylinderShape(SceneManager* scene, float length = 1.0f, float radius1 = 1.0f, float radius2 = 1.0f, int subdivisions = 1, int sides = 8, Vector2f tiling = Vector2f::Ones());

	private:
		float myLength;
		float myRadius1;
		float myRadius2;

		int mySubdivisions;
		int mySides;
		Vector2f myTiling;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////
	//! PYAPI
	class CY_API SphereShape: public Entity
	{
	public:
		//! PYAPI Convenience method for creating SphereShape
		static SphereShape* create(float radius, int subdivisions);

	public:
		SphereShape(SceneManager* scene, float radius = 1.0f, int subdivisions = 4, Vector2f tiling = Vector2f::Ones());

	private:
		float myRadius;
		int mySubdivisions;
		Vector2f myTiling;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////
	class CY_API PlaneShape: public Entity
	{
	public:
		//! Convenience method for creating PlaneShape
		static PlaneShape* create(float width, float height);

	public:
		PlaneShape(SceneManager* scene, float width = 1.0f, float height = 1.0f, Vector2f tiling = Vector2f::Ones());

	private:
		float myWidth;
		float myHeight;
		Vector2f myTiling;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////
	class CY_API BoxShape: public Entity
	{
	public:
		//! Convenience method for creating BoxShape
		static BoxShape* create(float width, float height, float depth);

	public:
		BoxShape(SceneManager* scene, float width = 1.0f, float height = 1.0f, float depth = 1.0, Vector2f tiling = Vector2f::Ones());

	private:
		float myWidth;
		float myHeight;
		float myDepth;
		Vector2f myTiling;
	};
};

#endif
