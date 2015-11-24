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
 ******************************************************************************/
#include "cyclops/CompositingLayer.h"
#include "cyclops/Uniforms.h"
#include "cyclops/Entity.h"

using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////
CompositingLayer::CompositingLayer():
	myShaderManager(new ShaderManager())
{
	myOutputNode = new osg::Group();

	// Since there is no compositor loaded now, directly attach the root group
	// to the output node.
	myOutputNode->addChild(myRoot);
}

///////////////////////////////////////////////////////////////////////////////
CompositingLayer::~CompositingLayer()
{
	reset();
}

///////////////////////////////////////////////////////////////////////////////
void CompositingLayer::addEntity(Entity* e)
{
	SceneLayer::addEntity(e);
	e->setShaderManager(myShaderManager);
}

///////////////////////////////////////////////////////////////////////////////
void CompositingLayer::reset()
{
	if(myCompositor != NULL)
	{
		myCompositor->removeChild(myRoot);
		myOutputNode->removeChild(myCompositor);
		myOutputNode->addChild(myRoot);
		myCompositor = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
void CompositingLayer::loadCompositor(const String& filename)
{
	if(myCompositor != NULL) reset();

	myCompositor = readEffectFile(filename);
	if(myCompositor != NULL)
	{
		myOutputNode->removeChild(myRoot);
		myOutputNode->addChild(myCompositor);
		myCompositor->addChild(myRoot);
	}
}

///////////////////////////////////////////////////////////////////////////////
void CompositingLayer::setPassActive(const String& passName, bool active)
{
	if(myCompositor != NULL)
	{
		myCompositor->setPassActivated(passName, active);
	}
}

///////////////////////////////////////////////////////////////////////////////
bool CompositingLayer::isPassActive(const String& passName)
{
	if(myCompositor != NULL)
	{
		return myCompositor->getPassActivated(passName);
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
cyclops::Uniform* CompositingLayer::getUniform(const String& name)
{
	if(myCompositor != NULL)
	{
		osg::Uniform* u = myCompositor->getUniform(name);
		if(u != NULL)
		{
			return new cyclops::Uniform(u);
		}
	}
	return NULL;
}
