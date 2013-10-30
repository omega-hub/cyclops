/**************************************************************************************************
 * THE OMEGA LIB PROJECT
 *-------------------------------------------------------------------------------------------------
 * Copyright 2010-2013		Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-------------------------------------------------------------------------------------------------
 * Copyright (c) 2010-2013, Electronic Visualization Laboratory, University of Illinois at Chicago
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
#include "cyclops/EffectNode.h"
#include "cyclops/SceneManager.h"

#include <osgFX/Technique>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include<osg/BlendFunc>

using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////////////////////////
class Technique: public osgFX::Technique
{
public:
	Technique(const String& definition, EffectNode* fx): myDefinition(definition), myFxNode(fx)
	{ define_passes(); }

    /// Validate.
    bool validate(osg::State&) const { return true; }
protected:

	///////////////////////////////////////////////////////////////////////////////////////////////
	virtual void define_passes()
	{
		// If an effect definition is specified, clear the effect materials and recreate them use
		// the effect definition string.
		if(myDefinition != "")
		{
			myFxNode->clearMaterials();

			Vector<String> passes = StringUtils::split(myDefinition, "|");

			for(Vector<String>::iterator iter = passes.begin(); iter != passes.end(); iter++)
			{
				StringUtils::trim(*iter);
				osg::StateSet* ss = new osg::StateSet();
				Material* mat = new Material(ss, myFxNode->getSceneManager());
				if(mat->parse(*iter)) 
				{
					myFxNode->addMaterial(mat);
					addPass(ss);

					// Renrer bin hack: addPass resets the render bin for the stateset, so we force
					// the opaque / transparent renderbin settings here. Re-calling setTransparent
					// will make sure the render bin is correct.
					mat->setTransparent(mat->isTransparent());
				}
			}
		}
		else
		{
			// No definition specified: create passes using the existing materials in the effect node.
			for(int i = 0; i < myFxNode->getMaterialCount(); i++)
			{
				addPass(myFxNode->getMaterial(i)->getStateSet());
			}
		}
	}

private:
	String myDefinition;
	EffectNode* myFxNode;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EffectNode::EffectNode(SceneManager* sm):
	mySceneManager(sm)
{
	dirtyTechniques();
	//myMaterial = new Material(getOrCreateStateSet(), sm);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EffectNode::~EffectNode() 
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Material* EffectNode::getMaterial(unsigned int index)
{
	return myMaterials[index];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EffectNode::addMaterial(Material* mat)
{
	myMaterials.push_back(mat);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EffectNode::clearMaterials()
{
	myMaterials.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EffectNode::setDefinition(const String& definition)
{
	myDefinition = definition;
	
	dirtyTechniques();
	
	// Force a node traversal to regenerate techniques
	osg::NodeVisitor nv;
	traverse(nv);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EffectNode::define_techniques()
{
	myCurrentTechnique = new Technique(myDefinition, this);
	this->addTechnique(myCurrentTechnique);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
void EffectNode::setShaderManager(ShaderManager* sm)
{
	myShaderManager = sm;
	foreach(Material* m, myMaterials)
	{
		m->setShaderManager(sm);
	}
}

