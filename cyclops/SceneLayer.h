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
 *	A scene layer is an abstract class that groups entities together for a
 *  variety of purposes: lighting, clipping, LOD and so on. SceneLayers can form
 *	a hyerarchy similar to the scene node tree, but the scene layer tree is used
 *	to represent properties of the scene different than spatial transformations.
 ******************************************************************************/
#ifndef __CY_SCENE_LAYER__
#define __CY_SCENE_LAYER__

#include "cyclopsConfig.h"
#include <omega.h>
#include <osg/Group>
#include <osg/NodeCallback>

namespace cyclops {
    using namespace omega;
    
    class Entity;
    
    ///////////////////////////////////////////////////////////////////////////
    //!	A scene layer is an abstract class that groups entities together for a
    //! variety of purposes: lighting, clipping, LOD and so on. SceneLayers 
    //!	can form a hyerarchy similar to the scene node tree, but the scene 
    //!	layer tree is used to represent properties of the scene different than 
    //! spatial transformations.
    class CY_API SceneLayer: public ReferenceType, public SceneNodeListener
    {
    friend class Entity;
    public:
        //! Use this flag on cameras that should only draw layers 
        //! explicitly attached to them.
        static const uint CameraDrawExplicitLayers;

        //! Layer Ids, to use with Layer.setId and Camera.setFlag to associate
        //! cameras to layers. Alternative system to Layer.setCamera for layers
        //! that need to be enabled for multiple cameras.
        static const uint Layer1;
        static const uint Layer2;
        static const uint Layer3;
        static const uint Layer4;
        static const uint Layer5;
        static const uint Layer6;
        static const uint Layer7;
        static const uint Layer8;


    public:
        SceneLayer();
        virtual ~SceneLayer();

        //! Add a sub-layer 
        virtual void addLayer(SceneLayer* layer);
        //! remove a sub-layer
        virtual void removeLayer(SceneLayer* layer);
        List< Ref<SceneLayer> >& getLayers();
        SceneLayer* getParentLayer() { return myParent; }

        //! @internal SceneNodeListener overrides
        //! These methods are needed to handle entities when they get attached
        //! or detached from the scene
        virtual void onAttachedToScene(SceneNode* source);
        virtual void onDetachedFromScene(SceneNode* source);

        virtual osg::Group* getOsgNode() { return myRoot; }

        //! Invokes the updateLayer function on this layer and all sub-layers
        void update();

        //! The camera that will draw this layer. If no camera is specified,
        //! all cameras will draw this layer.
        void setCamera(Camera* cam);
        Camera* getCamera();

        //! Gets or sets the layer Id.
        //! @remarks Layer ids are an alternative system to Layer.setCamera for layers
        //! that need to be enabled for multiple cameras.
        void setId(uint id);
        uint getId();

    protected:
        virtual void updateLayer() {}
        virtual void addEntity(Entity* e);
        virtual void removeEntity(Entity* e);

    protected:
        SceneLayer* myParent;
        Ref<osg::Group> myRoot;
        Ref<osg::NodeCallback> myCullCallback;

        // We keep weak entity pointers around, so entities with no references
        // can be deleted.
        List< Entity* > myEntities;
        List< Ref<SceneLayer> > myLayers;

        // The camera that will draw this layer. If no camera is specified,
        // all cameras will draw this layer.
        Ref<Camera> myCamera;

        // To use with Layer<n> scene ids.
        uint myId;
    };	
    
    ///////////////////////////////////////////////////////////////////////////
    inline List< Ref<SceneLayer> >& SceneLayer::getLayers()
    { return myLayers; }

    ///////////////////////////////////////////////////////////////////////////
    inline void SceneLayer::setCamera(Camera* cam)
    { myCamera = cam; }

    ///////////////////////////////////////////////////////////////////////////
    inline Camera* SceneLayer::getCamera()
    { return myCamera; }

    ///////////////////////////////////////////////////////////////////////////
    inline void SceneLayer::setId(uint id)
    { myId = id; }

    ///////////////////////////////////////////////////////////////////////////
    inline uint SceneLayer::getId()
    { return myId; }
};

#endif