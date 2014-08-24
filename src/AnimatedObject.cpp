/******************************************************************************
* THE OMEGA LIB PROJECT
*-----------------------------------------------------------------------------
* Copyright 2010-2014		Electronic Visualization Laboratory,
*							University of Illinois at Chicago
* Authors:
*  Alessandro Febretti		febret@gmail.com
*-----------------------------------------------------------------------------
* Copyright (c) 2010-2014, Electronic Visualization Laboratory,
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
* A class to handle entities that contain animations.
******************************************************************************/
#include <osgUtil/Optimizer>
#include <osgDB/Archive>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgAnimation/Animation>

#include "cyclops/SceneManager.h"
#include "cyclops/AnimatedObject.h"

using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////////////////////////
struct AnimationManagerFinder : public osg::NodeVisitor 
{ 
    osgAnimation::BasicAnimationManager* am; 
    
    AnimationManagerFinder(): am(NULL) {setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN); } 
    
    void apply(osg::Node& node) { 
    
        if (am != NULL) 
            return; 
    
        if (node.getUpdateCallback()) {        
            am = dynamic_cast<osgAnimation::BasicAnimationManager*>(node.getUpdateCallback()); 
            return; 
        } 
        
        traverse(node); 
    } 
}; 

///////////////////////////////////////////////////////////////////////////////////////////////////
AnimatedObject* AnimatedObject::create(const String& modelName)
{
    return new AnimatedObject(SceneManager::instance(), modelName);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
AnimatedObject::AnimatedObject(SceneManager* scene, const String& modelName):
    Entity(scene),
        mySceneManager(scene), 
        myOsgSwitch(NULL), 
        myCurrentModelIndex(0),
        myAnimationManager(NULL),
        myAnimations(NULL),
        myCurAnimation(NULL),
        myNeedStartAnimation(false),
        myCurAnimationId(-1)
{
    myModel = scene->getModel(modelName);

    osg::Node* osgRoot = NULL;

    if(myModel != NULL)
    {
        if(myModel->numNodes == 1)
        {
            // Single model asset
            osgRoot = myModel->nodes[0];
        }
        else
        {
            // Multi model asset
            myOsgSwitch = new osg::Switch();
            int i = 0;
            foreach(osg::Node* n, myModel->nodes)
            {
                myOsgSwitch->addChild(n);
                myOsgSwitch->setChildValue(n, i++);
            }
            osgRoot = myOsgSwitch;
        }

        initialize(osgRoot);

        // Traverse this entity hierarchy to find animations.
        AnimationManagerFinder amf;
        osgRoot->accept(amf);
        if(amf.am != NULL)
        {
            myAnimationManager = amf.am;
            myAnimations = &myAnimationManager->getAnimationList();
            ofmsg("AnimatedObject %1%: found %2% animations.", %getName() %getNumAnimations());

            // DO not play an anymation by default anymore.
            //loopAnimation(0);
        }
    }
    else
    {
        ofwarn("AnimatedObject::AnimatedObject: could not create static object: model not found - %1%", %modelName);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AnimatedObject::updateTraversal(const UpdateContext& context)
{
    myCurTime = context.time;

    if(myCurAnimation != NULL)
    {
        if(myNeedStartAnimation)
        {
            myNeedStartAnimation = false;
            myAnimationManager->playAnimation(myCurAnimation);
        }
        // See if the current animation has finished playing.
        if(!myAnimationManager->isPlaying(myCurAnimation))
        {
            ofmsg("AnimatedObject %1%: animation ended", %getName());

            myCurAnimation = NULL;
            myCurAnimationId = -1;
            if(myOnAnimationEnded.length() > 0)
            {
                SystemManager::instance()->getScriptInterpreter()->eval(myOnAnimationEnded);
            }
        }
    }
    SceneNode::updateTraversal(context);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AnimatedObject::setCurrentModelIndex(int index)
{
    if(myOsgSwitch != NULL && index < getNumModels())
    {
        myCurrentModelIndex = index;
        myOsgSwitch->setSingleChildOn(index);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int AnimatedObject::getCurrentModelIndex()
{
    if(myOsgSwitch != NULL)
    {
        return myCurrentModelIndex;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool AnimatedObject::hasAnimations()
{
    return myAnimationManager != NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int AnimatedObject::getNumAnimations()
{
    if(hasAnimations())
    {
        return myAnimations->size();
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AnimatedObject::playAnimation(int id)
{
    if(hasAnimations())
    {
        if(id < getNumAnimations())
        {
            myCurAnimationId = id;
            myCurAnimation = myAnimations->at(id);
            myCurAnimation->setPlayMode(osgAnimation::Animation::ONCE);
            myCurAnimation->setStartTime(myCurTime);
            myNeedStartAnimation = true;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AnimatedObject::loopAnimation(int id)
{
    if(hasAnimations())
    {
        if(id < getNumAnimations())
        {
            myCurAnimationId = id;
            myCurAnimation = myAnimations->at(id);
            myCurAnimation->setPlayMode(osgAnimation::Animation::LOOP);
            myNeedStartAnimation = true;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AnimatedObject::pauseAnimation()
{
    if(hasAnimations() && myCurAnimation != NULL)
    {
        myAnimationManager->stopAnimation(myCurAnimation);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AnimatedObject::stopAllAnimations()
{
    if(hasAnimations())
    {
        myAnimationManager->stopAll();
        myCurAnimation = NULL;
        myCurAnimationId = -1;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int AnimatedObject::getCurAnimation()
{
    return myCurAnimationId;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
float AnimatedObject::getAnimationLength(int id)
{
    if(hasAnimations() && id < getNumAnimations())
    {
        osgAnimation::Animation* anim = myAnimations->at(id);
        anim->computeDuration(); // osgAnimation: Duration must be computed before calling getDuration()
        return anim->getDuration();
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
float AnimatedObject::getAnimationStart(int id)
{
    if(hasAnimations() && id < getNumAnimations())
    {
        osgAnimation::Animation* anim = myAnimations->at(id);
        return anim->getDuration();
    }
    return 0.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AnimatedObject::setAnimationStart(int id, float time)
{
    if(hasAnimations() && id < getNumAnimations())
    {
        osgAnimation::Animation* anim = myAnimations->at(id);
        anim->setStartTime(time);
    }
}
