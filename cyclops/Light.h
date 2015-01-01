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
 *	A light that can be added to a cyclops scene.
 ******************************************************************************/
#ifndef __CY_LIGHT__
#define __CY_LIGHT__

#include "cyclopsConfig.h"
#include "ShadowMap.h"

#include <osg/Group>
#include <osg/Light>
#include <osg/LightSource>

#define OMEGA_NO_GL_HEADERS
#include <omega.h>
#include <omegaOsg/omegaOsg.h>
#include <omegaToolkit.h>

namespace cyclops {
    using namespace omega;
    using namespace omegaOsg;

    class SceneLoader;
    class SceneManager;
    class AnimatedObject;
    class LightingLayer;
    class LightInstance;

    ///////////////////////////////////////////////////////////////////////////
    class CY_API Light: public SceneNode
    {
    friend class SceneManager;
    friend class LightInstance;
    public:
        enum LightType { Point, Directional, Spot, Custom };
        enum ShadowRefreshMode { OnFrame, OnLightMove, Manual };

        //! Convenience method for creating Light instances
        static Light* create();
    
    public:
        Light(SceneManager* scene);
        virtual ~Light();

        ShadowMap* getShadow() { return myShadow; }
        void setShadow(ShadowMap* s);
        void setShadowRefreshMode(ShadowRefreshMode srm);

        //! Sets or gets the layer this light is applied to
        virtual void setLayer(LightingLayer* layer);
        virtual LightingLayer* getLayer();

        //! Create a new instance of this light and attach it to the specified
        //! node.
        LightInstance* createInstance(osg::Group* rootNode);
        void destroyInstance(LightInstance* i);

        void setColor(const Color& value) { myColor = value; }
        const Color& getColor() { return myColor; }

        void setAmbient(const Color& value) { myAmbient = value; }
        const Color& getAmbient() { return myAmbient; }

        void setEnabled(bool value);
        bool isEnabled() { return myEnabled; }

        void setAttenuation(float constant, float linear, float quadratic) 
        { 
            myAttenuation[0] = constant; 
            myAttenuation[1] = linear; 
            myAttenuation[2] = quadratic; 
        }
        const Vector3f& getAttenuation() { return myAttenuation; }

        void setLightType(LightType type);
        LightType getLightType() { return myType; }

        void setLightDirection(const Vector3f& value) { myLightDirection = value; }
        Vector3f getLightDirection() { return myLightDirection; }

        void setSpotExponent(float value) { mySpotExponent = value; }
        float getSpotExponent() { return mySpotExponent; }
        void setSpotCutoff(float value) { mySpotCutoff = value; }
        float getSpotCutoff() { return mySpotCutoff; }

        void setLightFunction(const String& function) { myLightFunction = function; }
        String getLightFunction() { return myLightFunction; }

        virtual void updateTraversal(const UpdateContext& context);

        //! Forces a regeneration of shaders using this light.
        void requestShaderUpdate();

    private:
        SceneManager* mySceneManager;
        LightingLayer* myLayer;

        Color myColor;
        Color myAmbient;
        bool myEnabled;
        Vector3f myAttenuation;

        float mySpotExponent;
        float mySpotCutoff;

        Vector3f myLightDirection;

        // Light instances 
        // (will replace osg light stuff)
        List< Ref<LightInstance> > myInstances;

        LightType myType;
        String myLightFunction;

        // Shadow stuff
        Ref<ShadowMap> myShadow;
        ShadowRefreshMode myShadowRefreshMode;
        Vector3f myLastShadowPos;
    };

    ///////////////////////////////////////////////////////////////////////////
    //! Represents an instance of a light in the scene. A single light may have
    //! Multiple instances, each one being represented by a different 
    //! OpenGL light.
    class CY_API LightInstance: public ReferenceType
    {
    friend class Light;
    public:
        ~LightInstance();

        //! @internal update the osg light parameters. Returns true if shaders
        //! need to be recompiled due to light changes.
        bool update();

        Light* getLight() { return myLight; }
        int getLightIndex() { return myIndex; }
        void setLightIndex(int index);

        osg::Light* getOsgLight() { return myOsgLight; }

    private:
        //! Create a new light instance and attach it to the specified
        //! osg group
        LightInstance(Light* l, osg::Group* root);

        void requestShaderUpdate() { myShaderUpdateNeeded = true; }
    private:
        Ref<Light> myLight;
        int myIndex;

        // osg light stuff.
        Ref<osg::Group> myGroup;
        Ref<osg::Light> myOsgLight;
        Ref<osg::LightSource> myOsgLightSource;

        bool myShaderUpdateNeeded;
    };
};

#endif
