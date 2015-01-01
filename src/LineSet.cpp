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
#include "cyclops/LineSet.h"
#include "cyclops/SceneManager.h"

#include<osgwTools/Shapes.h>

using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////
LineSet* LineSet::create()
{
	return new LineSet(SceneManager::instance());
}

///////////////////////////////////////////////////////////////////////////////
LineSet::Line::Line(LineSet* owner):
	myOwner(owner),
	myGeode(NULL),
	myMaterial(NULL),
	myStartPoint(Vector3f(-1, 0, 0)),
	myEndPoint(Vector3f(1, 0, 0)),
	myThickness(0.01f)
{
	myGeode = new osg::Geode();
	myTransform = new osg::PositionAttitudeTransform();

	myTransform->addChild(myGeode);

	osg::Geometry* geome = new osg::Geometry();
	myGeode->addDrawable(geome);

	osg::Vec2s subdiv(1, 8);
	osgwTools::makeOpenCylinder(1.0f, 0.5f, 0.5f, subdiv, geome);

	myThicknessUniform = new osg::Uniform("unif_Thickness", (float)myThickness);

	osg::StateSet* ss = myGeode->getOrCreateStateSet();
	ss->addUniform( myThicknessUniform );

	updateTransform();
}

///////////////////////////////////////////////////////////////////////////////
void LineSet::Line::updateTransform()
{
	Vector3f dir = (myEndPoint - myStartPoint);
	Vector3f midpoint = (myStartPoint + myEndPoint) / 2;
	myLength = dir.norm();
	dir = dir / myLength;
	Quaternion o = Math::buildRotation(Vector3f(0, 0, 1), dir, Vector3f(0, 1, 0));
	osg::Quat qo(o.x(), o.y(), o.z(), o.w());
	myTransform->setAttitude(qo);
	myTransform->setScale(osg::Vec3d(myThickness, myThickness, myLength));
	myTransform->setPosition(osg::Vec3d(myStartPoint.x(), myStartPoint.y(), myStartPoint.z()));
	myOwner->requestBoundingBoxUpdate();
}

///////////////////////////////////////////////////////////////////////////////
void LineSet::Line::setThickness(float value)
{
	myThickness = value;
	myThicknessUniform->set(myThickness);
	myTransform->setScale(osg::Vec3d(myThickness, myThickness, myLength));
	myOwner->requestBoundingBoxUpdate();
}

///////////////////////////////////////////////////////////////////////////////
LineSet::LineSet(SceneManager* sm): Entity(sm)
{
	myLineGroup = new osg::Group();
	initialize(myLineGroup);
}

///////////////////////////////////////////////////////////////////////////////
LineSet::Line* LineSet::addLine()
{
	Line* res = NULL;
	if(!myDisposedLines.empty())
	{
		res = myDisposedLines.back();
		myDisposedLines.pop_back();
	}
	else
	{
		res = new Line(this);
	}
	myActiveLines.push_back(res);
	myLineGroup->addChild(res->getOsgNode());
	requestBoundingBoxUpdate();

	return res;
}

///////////////////////////////////////////////////////////////////////////////
void LineSet::removeLine(Line* line)
{
	myLineGroup->removeChild(line->getOsgNode());
	myActiveLines.remove(line);
	myDisposedLines.push_back(line);
	requestBoundingBoxUpdate();
}
