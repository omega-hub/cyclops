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
#ifndef __CY_RIGID_BODY__
#define __CY_RIGID_BODY__

#include "cyclopsConfig.h"

// forward declarations
class btRigidBody;
class btCollisionShape;
class btVector3;
struct btDefaultMotionState;

namespace cyclops {
    using namespace omega;
    using namespace omegaOsg;

    class SceneManager;
    class Entity;

    ///////////////////////////////////////////////////////////////////////////
    class CY_API RigidBody: public ReferenceType
    {
    public:
        enum RigidBodyType { Box, Sphere, Cylinder, Plane };

    public:
        RigidBody(Entity* e);
        virtual ~RigidBody();

        void initialize(RigidBodyType type, float mass);

        //! updates the attached entity transform with the transform coming
        //! from the physics simlation. Called by Entity::updateTraversal
        void updateEntity();

        //! updates the rigid body transform from the entity
        void sync();

        bool isEnabled();
        void setEnabled(bool value);

        void setUserControlled(bool value);
        bool isUserControlled();

    // Forces
    void applyCentralForce(const Vector3f& force);
    void applyForce(const Vector3f& force, const Vector3f& relPos);
    void applyCentralImpulse(const Vector3f& impulse);
    void applyImpulse(const Vector3f& impulse, const Vector3f& relPos);
    void setLinearVelocity(const Vector3f& linearVelocity);
    void setAngularVelocity(const Vector3f& angularVelocity);

    void setFriction(float friction);
    float getFriction();
    void setRollingFriction(float friction);
    float getRollingFriction();

    private:
        Entity* myEntity;

        bool myEnabled;
        bool myUserControlled;
        RigidBodyType myType;
        float myMass;

        // List of possible rigid body shapes (not sure this is the best
        // implementation).
        btCollisionShape* myCollisionShape;
        //btBoxShape* myBoxShape;
        //btSphereShape* mySphereShape;

        btRigidBody* myRigidBody;
        btDefaultMotionState* myMotionState;
    };

    ///////////////////////////////////////////////////////////////////////////
    inline bool RigidBody::isEnabled()
    { return myEnabled; }

    ///////////////////////////////////////////////////////////////////////////
    inline void RigidBody::setUserControlled(bool value)
    {
        myUserControlled = value;
    }

    ///////////////////////////////////////////////////////////////////////////
    inline bool RigidBody::isUserControlled()
    {
        return myUserControlled;
    }

};

#endif
