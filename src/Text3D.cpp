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
 *	A 3D text entity 
 ******************************************************************************/
#include "cyclops/Text3D.h"
#include "cyclops/SceneManager.h"

#include <osgUtil/TangentSpaceGenerator>
#include <osgwTools/Shapes.h>
#include<osgText/Text>

using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////
Text3D* Text3D::create(const String& font, float size, const String& text)
{
	Text3D* txt = new Text3D(SceneManager::instance());
	txt->setFont(font);
	txt->setFontSize(size);
	txt->setText(text);
	return txt;
}

///////////////////////////////////////////////////////////////////////////////
Text3D::Text3D(SceneManager* scene):
	Entity(scene)
{
	osg::Geode* txtGeode = new osg::Geode();
	myTextDrawable = new osgText::Text();
	myTextDrawable->setColor(osg::Vec4(1,1,1,1));
	txtGeode->addDrawable(myTextDrawable);

	initialize(txtGeode);

	// Lighting disabled on text objects.
	getMaterial()->setLit(false);
	// Make sure cube map textures are disabled, or text color will be 
	// modulated by them.
	// NOTE: The SKyBox class uses texture stage 3 for the cube map texture.
	getMaterial()->getStateSet()->setTextureMode(3, GL_TEXTURE_CUBE_MAP, osg::StateAttribute::OFF);
}

///////////////////////////////////////////////////////////////////////////////
void Text3D::setFont(const String& fontfile)
{
	myTextDrawable->setFont(fontfile);
	//myTextDrawable
}

///////////////////////////////////////////////////////////////////////////////
void Text3D::setFontSize(float size)
{
	myTextDrawable->setCharacterSize(size);
	//myTextDrawable->setCharacterSizeMode(
	//	osgText::TextBase::OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT);
	//myTextDrawable->set
	//myTextDrawable
}

///////////////////////////////////////////////////////////////////////////////
void Text3D::setFontResolution(int resolution)
{
	myTextDrawable->setFontResolution(resolution, resolution);
}

///////////////////////////////////////////////////////////////////////////////
void Text3D::setText(const String& text)
{
	myTextDrawable->setText(text);
	requestBoundingBoxUpdate();
}

///////////////////////////////////////////////////////////////////////////////
void Text3D::setFixedSize(bool value)
{
	if(value)
	{
		myTextDrawable->setCharacterSizeMode(osgText::TextBase::SCREEN_COORDS);
	}
	else
	{
		myTextDrawable->setCharacterSizeMode(osgText::TextBase::OBJECT_COORDS);
	}
}

///////////////////////////////////////////////////////////////////////////////
void Text3D::setColor(const Color& color)
{
	myTextDrawable->setColor(COLOR_TO_OSG(color));
}

