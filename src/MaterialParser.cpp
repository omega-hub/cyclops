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
 *	A parser that applies string material definitions to materials.
 ******************************************************************************/
#include "cyclops/MaterialParser.h"

#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include<osg/BlendFunc>

#include <iostream>

using namespace cyclops;
using namespace libconfig;

///////////////////////////////////////////////////////////////////////////////
bool processDefaultArguments(Material* mat, ArgumentHelper& ah, const String& def)
{
	String effectName;
	String variation = "";
	bool transparent = false;
	bool additive = false;
	bool disableDepth = false;
	bool disableCull = false;
	bool wireframe = false;
	double offset = 0;
	double shininess = 10;
	double gloss = 0;
	String diffuse = "";
	String emissive = "";
	String normal = "";

	ah.newString("effectName", 
		"the effect name", effectName);
	ah.newFlag('t', "transparent", 
		"enable transparency for this effect", transparent);
	ah.newFlag('a', "additive", 
		"enable additive blending for this effect", additive);
	ah.newFlag('D', "disable-depth", 
		"disable depth testing for this effect", disableDepth);
	ah.newFlag('C', "disable-cull", 
		"disable back face culling", disableCull);
	ah.newNamedString('v', "variation", "variation", 
		"effect variation", variation);
	ah.newNamedDouble('o', "offset", "polygon offset", 
		"enables and specifies the polygon offset", offset);
	ah.newFlag('w', "wireframe", 
		"enables wireframe", wireframe);
	ah.newNamedDouble('s', "shininess", "shininess", 
		"specular power - defines size of specular highlights", shininess);
	ah.newNamedDouble('g', "gloss", "gloss", 
		"gloss [0 - 1] - reflectivity of surface", gloss);
	ah.newNamedString('d', "diffuse", "diffuse material", 
		"diffuse material color or texture", diffuse);
	ah.newNamedString('e', "emissive", "emissive material", 
		"emissive material color or texture", emissive);
	ah.newNamedString('n', "normal", "normal texture", 
		"normal texture file name", normal);

	bool help = false;
	ah.newFlag('?', "help", "prints help", help);
	ah.process(def.c_str());

	if(help)
	{
		osg::StateSet* ss = new osg::StateSet();
		ah.writeUsage(cout);
		return false;
	}

	// Set the material program
	if(variation == "")
	{
		mat->setProgram(effectName);
	}
	else
	{
		mat->setProgram(effectName + " " + variation);
	}

	mat->setAdditive(additive);
	mat->setTransparent(transparent);
	mat->setDepthTestEnabled(!disableDepth);
	mat->setDoubleFace(disableCull);

	if(wireframe) mat->setWireframe(true);
	if(offset != 0) mat->setPolygonOffset(2.0f, offset);

	if(diffuse != "" | emissive != "")
	{
		// If a color is empty, set it to black.
		if(diffuse == "") diffuse = "#000000ff";
		if(emissive == "") emissive = "#000000ff";
		if(Color::isValidColor(diffuse) && Color::isValidColor(emissive))
		{
			mat->setColor(Color(diffuse), Color(emissive));
		}
		else
		{
			mat->setDiffuseTexture(diffuse);
		}
	}

	if(normal != "")
	{
		// This needs to be done on the program asset.
		//asset->program->addBindAttribLocation("attrib_Tangent", 6);
		mat->setNormalTexture(normal);
	}

	mat->setShininess(shininess);
	mat->setGloss(gloss);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool MaterialParser::parseMaterialString(Material* mat, const String& def)
{
	libconfig::ArgumentHelper ah;
	return processDefaultArguments(mat, ah, def);
}
