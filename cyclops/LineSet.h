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
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this 
 * list of conditions and the following disclaimer. Redistributions in binary 
 * form must reproduce the above copyright notice, this list of conditions and 
 * the following disclaimer in the documentation and/or other materials 
 * provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE  GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY,  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *-----------------------------------------------------------------------------
 * What's in this file
 *	The LineSet entity: used to draw multiple 3d lines. Each line is actually
 *	a cylinder. Lines are grouped together to increase performance.
 *****************************************************************************/
#ifndef __CY_LINE_SET__
#define __CY_LINE_SET__

#include "Entity.h"

#include<osg/PositionAttitudeTransform>
#include<osg/Material>

namespace cyclops {
	using namespace omega;
	using namespace omegaOsg;

	///////////////////////////////////////////////////////////////////////////
	class CY_API LineSet: public Entity
	{
	public:
		class CY_API Line: public ReferenceType
		{
		public:
			Line(LineSet* owner);

			void setStart(const Vector3f& value) { myStartPoint = value; updateTransform(); }
			void setEnd(const Vector3f& value) { myEndPoint = value; updateTransform(); }
			osg::Node* getOsgNode() { return myTransform; }
			void setThickness(float value);
			float getThickness() { return myThickness; }


		private:
			LineSet* myOwner;

			Vector3f myStartPoint;
			Vector3f myEndPoint;
			osg::Geode* myGeode;
			osg::PositionAttitudeTransform* myTransform;
			osg::Material* myMaterial;
			osg::Uniform* myThicknessUniform;
			float myThickness;
			float myLength;

		private:
			void updateTransform();
		};

	public:
		static LineSet* create();
		LineSet(SceneManager* sm);
		Line* addLine();
		void removeLine(Line* line);

	private:
		osg::Group* myLineGroup;

		List<Line*> myActiveLines;
		List<Line*> myDisposedLines;
	};

};

#endif
