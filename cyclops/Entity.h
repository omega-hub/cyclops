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
 * Contains code for the Entity class. All drawable 3D objects in the cyclops 
 * framework derive from Entity.
 ******************************************************************************/
#ifndef __CY__ENTITY__
#define __CY__ENTITY__

#include "cyclopsConfig.h"
#include "EffectNode.h"
#include "Uniforms.h"
#include "RigidBody.h"

#include <osg/Group>

#define OMEGA_NO_GL_HEADERS
#include <omega.h>
#include <omegaOsg/omegaOsg.h>
#include <omegaToolkit.h>

namespace cyclops {
    using namespace omega;
    using namespace omegaOsg;

    class SceneManager;
    class SceneLayer;
    class ShaderManager;

    ///////////////////////////////////////////////////////////////////////////
    //! Encapsulates an osg node (or nodes) and offers a few additional 
    //! functionality for them, like shadow casting management, effects and 
    //! shaders and context menus.
    class CY_API Entity: public SceneNode
    {
    public:
        Entity(SceneManager* scene);
        virtual ~Entity();

        SceneManager* getSceneManager() { return mySceneManager; }

        osg::Node* getOsgNode() { return myEffect; }
        //SceneNode* getSceneNode() { return mySceneNode; }

        SceneLayer* getLayer();
        void setLayer(SceneLayer* layer);

        //! Visuals
        //@{
        bool hasEffect();
        void setEffect(const String& effectDefinition);
        Material* getMaterial();
        Material* getMaterialByIndex(unsigned int index);
        int getMaterialCount();
        void addMaterial(Material* mat);
        //! Removes a material from this entity.
        void removeMaterial(Material* mat);
        void clearMaterials();
        //! Sets the shader manager used by this entity to find shaders used
        //! my the entity materials. Default shader manager is the scene manager
        void setShaderManager(ShaderManager* sm);
        //@}

        void castShadow(bool value);
        bool doesCastShadow();
        //! Enables or disables frustum culling for this entity. If culling is
        //! enabled (the default state), this entity will be culled when its
        //! bounding box does not intersect the camera frustum.
        void setCullingActive(bool value);
        bool isCullingActive();

        //! Enables or disables point-mode ray hit computation for this entity.
        //! When enabled, ray intersections with this entity will be computed
        //! based on its vertices, instead of its faces.
        void setPointIntersectionEnabled(bool value);
        bool isPointIntersectionEnabled();


        //! Piece management
        //@{
        vector<String> listPieces(const String& path);
        Entity* getPiece(const String& path);
        //@}

        //! Returns the rigid body object describing the physical properties
        // of this entity.
        RigidBody* getRigidBody();

        virtual void updateTraversal(const UpdateContext& context);

    protected:
        void initialize(osg::Node* node);
        //! Used by the piece functions to find a named group inside the object.
        osg::Group* findSubGroup(const String& path);

    private:
        Ref<SceneManager> mySceneManager;

        // Normal pointer to avoid circular refs.
        SceneLayer*	myLayer;

        Ref<osg::Node> myOsgNode;
        //SceneNode* mySceneNode;
        Ref<OsgSceneObject> myOsgSceneObject;

        Ref<EffectNode> myEffect;

        bool myCastShadow;
        bool myCullingActive;

        Ref<RigidBody> myRigidBody;
    };

    ///////////////////////////////////////////////////////////////////////////
    inline RigidBody* Entity::getRigidBody()
    { return myRigidBody; }

    ///////////////////////////////////////////////////////////////////////////
    inline SceneLayer* Entity::getLayer()
    { return myLayer; }

    ///////////////////////////////////////////////////////////////////////////
    inline void Entity::setPointIntersectionEnabled(bool value)
    {
        if(value) setFlag(OsgSceneObject::SceneNodeHitPointsFlag);
        else unsetFlag(OsgSceneObject::SceneNodeHitPointsFlag);
    }

    ///////////////////////////////////////////////////////////////////////////
    inline bool Entity::isPointIntersectionEnabled()
    {
        return isFlagSet(OsgSceneObject::SceneNodeHitPointsFlag);
    }
};

#endif
