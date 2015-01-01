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
#include <osg/Node>
#include <osg/Geometry>

#include "cyclops/ModelGeometry.h"

using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////
ModelGeometry::ModelGeometry(const String& name):
	myName(name)
{
	// create geometry and geodes to hold the data
	myNode = new osg::Geode();
	myGeometry = new osg::Geometry();
	osg::VertexBufferObject* vboP = myGeometry->getOrCreateVertexBufferObject();
	vboP->setUsage (GL_STREAM_DRAW);

	myVertices = new osg::Vec3Array();

	myGeometry->setUseDisplayList (false);
	myGeometry->setUseVertexBufferObjects(true);
	myGeometry->setVertexArray(myVertices);
  	myNode->addDrawable(myGeometry);
}


///////////////////////////////////////////////////////////////////////////////
int ModelGeometry::addVertex(const Vector3f& v)
{
	myVertices->push_back(osg::Vec3d(v[0], v[1], v[2]));
	myGeometry->dirtyBound();
	return myVertices->size() - 1;
}

///////////////////////////////////////////////////////////////////////////////
void ModelGeometry::setVertex(int index, const Vector3f& v)
{
	oassert(myVertices->size() > index);
	osg::Vec3f& c = myVertices->at(index);
	c[0] = v[0];
	c[1] = v[1];
	c[2] = v[2];
	myVertices->dirty();
}

///////////////////////////////////////////////////////////////////////////////
Vector3f ModelGeometry::getVertex(int index)
{
	oassert(myVertices->size() > index);
	const osg::Vec4d& c = myColors->at(index);
	return Vector3f(c[0], c[1], c[2]);
}

///////////////////////////////////////////////////////////////////////////////
int ModelGeometry::addColor(const Color& c)
{
	if(myColors == NULL)
	{
		myColors = new osg::Vec4Array();
		myGeometry->setColorArray(myColors);
		myGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	}
	myColors->push_back(osg::Vec4d(c[0], c[1], c[2], c[3]));
	return myColors->size() - 1;
}

///////////////////////////////////////////////////////////////////////////////
void ModelGeometry::setColor(int index, const Color& col)
{
	oassert(myColors != NULL && myColors->size() > index);
	osg::Vec4f& c = myColors->at(index);
	c[0] = col[0];
	c[1] = col[1];
	c[2] = col[2];
	c[3] = col[3];
	myColors->dirty();
}

///////////////////////////////////////////////////////////////////////////////
Color ModelGeometry::getColor(int index)
{
	oassert(myColors != NULL && myColors->size() > index);
	const osg::Vec4d& c = myColors->at(index);
	return Color(c[0], c[1], c[2], c[3]);
}

///////////////////////////////////////////////////////////////////////////////
void ModelGeometry::addPrimitive(ProgramAsset::PrimitiveType type, int startIndex, int endIndex)
{
	osg::PrimitiveSet::Mode osgPrimType;
	switch(type)
	{
	case ProgramAsset::Triangles:
		osgPrimType = osg::PrimitiveSet::TRIANGLES; break;
	case ProgramAsset::Points:
		osgPrimType = osg::PrimitiveSet::POINTS; break;
	case ProgramAsset::TriangleStrip:
		osgPrimType = osg::PrimitiveSet::TRIANGLE_STRIP; break;
	}
	myGeometry->addPrimitiveSet(new osg::DrawArrays(osgPrimType, startIndex, endIndex));
}

///////////////////////////////////////////////////////////////////////////////
void ModelGeometry::clear()
{
	myColors->clear();
	myVertices->clear();
	myGeometry->removePrimitiveSet(0, myGeometry->getNumPrimitiveSets());
	myGeometry->dirtyBound();
}
