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
 * The interface between entities and the Bullet Physics engine.
 ******************************************************************************/
#include "cyclops/SceneManager.h"
#include "cyclops/RigidBody.h"

#include "osgbCollision/CollisionShapes.h"
#include "osgbCollision/Utils.h"
#include "btBulletDynamicsCommon.h"

using namespace cyclops;
using namespace osgbCollision;

///////////////////////////////////////////////////////////////////////////////
RigidBody::RigidBody(Entity* e):
    myEntity(e),
    myCollisionShape(NULL),
    myRigidBody(NULL),
    myEnabled(false),
    myUserControlled(false)
{
}

///////////////////////////////////////////////////////////////////////////////
RigidBody::~RigidBody()
{
    setEnabled(false);
    myEntity = NULL;
    if(myCollisionShape)
    {
        delete myCollisionShape;
        myCollisionShape = NULL;
    }
    if(myRigidBody)
    {
        delete myRigidBody;
        myRigidBody = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::initialize(RigidBodyType type, float mass)
{
    myMass = mass;
    btVector3 inertia(0,0,0);
    if(type == Box || type == Sphere || type == Cylinder || type == Plane)
    {
        if (type==Box)
        {
            myCollisionShape = btBoxCollisionShapeFromOSG(myEntity->getOsgNode());
            //Vector3f he = myEntity->getBoundMaximum() - myEntity->getBoundMinimum();
            //myCollisionShape = new btBoxShape(btVector3(btVector3(he.x() / 2, he.y() / 2, he.z() / 2)));
        }
        else if (type==Sphere)
        {
            myCollisionShape = btSphereCollisionShapeFromOSG(myEntity->getOsgNode());
        }
        else if (type==Cylinder)
        {
            myCollisionShape = btCylinderCollisionShapeFromOSG(myEntity->getOsgNode());
        }
        else if (type==Plane)
        {
            myCollisionShape = btBoxCollisionShapeFromOSG(myEntity->getOsgNode());
        }
        if(myMass != 0)
        {
            myCollisionShape->calculateLocalInertia(myMass, inertia);
        }
        myMotionState = new btDefaultMotionState();
        //myMotionState->setWorldTransform(groundTransform);
        //myMotionState->setTransform(myGround);
        btRigidBody::btRigidBodyConstructionInfo rb( mass, myMotionState, myCollisionShape, inertia );
        myRigidBody = new btRigidBody( rb );
        myRigidBody->setActivationState(DISABLE_DEACTIVATION);
    }
    else
    {
        ofwarn("RigidBody::initialize: unsupported body shape %1% for entity %2%",
            %type %myEntity->getName());
        return;
    }

    // By default enabled newly-created rigid bodies.
    setEnabled(true);
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::setEnabled(bool value)
{
    myEnabled = value;
    if(myRigidBody != NULL)
    {
        SceneManager* sm = myEntity->getSceneManager();
        if(myEnabled)
        {
            sm->getDynamicsWorld()->addRigidBody(myRigidBody);
        }
        else
        {
            sm->getDynamicsWorld()->removeRigidBody(myRigidBody);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::updateEntity()
{
    if(myEnabled && myMotionState != NULL && !myUserControlled)
    {
        //btVector3 pos = myMotionState->m_graphicsWorldTrans.getOrigin();
        //ofmsg("%1% %2% %3%", %pos.x() %pos.y() %pos.z());
        btVector3 pos = myRigidBody->getCenterOfMassPosition();
        btQuaternion ort = myRigidBody->getCenterOfMassTransform().getRotation();
        myEntity->setPosition(pos.x(), pos.y(), pos.z());
        myEntity->setOrientation(ort.w(), ort.x(), ort.y(), ort.z());
    }
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::sync()
{
    if(myEnabled && myMotionState != NULL)
    {
        btTransform xform;
        const Vector3f& o = myEntity->getDerivedPosition();
        const Quaternion& r = myEntity->getDerivedOrientation();
        //xform.setOrigin(btVector3(o.x(), o.y(), o.z()));
        //xform.setRotation(btQuaternion(r.w(), r.x(), r.y(), r.z()));
        //myMotionState->setWorldTransform(xform);

        btTransform transform = myRigidBody->getCenterOfMassTransform();
        transform.setOrigin(btVector3(o.x(), o.y(), o.z()));
        transform.setRotation(btQuaternion(r.x(), r.y(), r.z(), r.w()));
        myRigidBody->setCenterOfMassTransform(transform);
    }
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::applyCentralForce(const Vector3f& force)
{
    myRigidBody->applyCentralForce(btVector3(force.x(), force.y(), force.z()));
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::applyForce(const Vector3f& force, const Vector3f& relPos)
{
  myRigidBody->applyForce(btVector3(force.x(), force.y(), force.z()), btVector3(relPos.x(), relPos.y(), relPos.z()));
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::applyCentralImpulse(const Vector3f& impulse)
{
  myRigidBody->applyCentralImpulse(btVector3(impulse.x(), impulse.y(), impulse.z()));
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::applyImpulse(const Vector3f& impulse, const Vector3f& relPos)
{
  myRigidBody->applyImpulse(btVector3(impulse.x(), impulse.y(), impulse.z()), btVector3(relPos.x(), relPos.y(), relPos.z()));
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::setLinearVelocity(const Vector3f& linearVelocity)
{
    myRigidBody->setLinearVelocity(btVector3(linearVelocity.x(), linearVelocity.y(), linearVelocity.z()));
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::setAngularVelocity(const Vector3f& angularVelocity)
{
  myRigidBody->setAngularVelocity(btVector3(angularVelocity.x(), angularVelocity.y(), angularVelocity.z()));
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::setFriction(float linear)
{
    myRigidBody->setFriction(linear);
}

///////////////////////////////////////////////////////////////////////////////
float RigidBody::getFriction()
{
    return myRigidBody->getFriction();
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::setRollingFriction(float rolling)
{
    myRigidBody->setRollingFriction(rolling);
}

///////////////////////////////////////////////////////////////////////////////
float RigidBody::getRollingFriction()
{
    return myRigidBody->getRollingFriction();
}
