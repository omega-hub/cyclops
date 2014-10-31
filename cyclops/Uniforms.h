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
 *	Classes to handle GLSL uniforms (Wrapping osg::Uniform)
 *****************************************************************************/
#ifndef __CY_UNIFORM__
#define __CY_UNIFORM__

#include "cyclopsConfig.h"

#include <osg/Uniform>

#define OMEGA_NO_GL_HEADERS
#include <omega.h>
#include <omegaOsg/omegaOsg.h>
#include <omegaToolkit.h>

namespace cyclops {
    using namespace omega;
    using namespace omegaOsg;

    ///////////////////////////////////////////////////////////////////////////
    class CY_API Uniform: public ReferenceType
    {
    public:
        enum Type { Int, Float, Vector2f, Vector3f, Color };
        static Uniform* create(const String& name, Type type, uint elements);

    public:
        //! Create a cyclops uniform encapsulating an osg uniform. The uniform
        //! type and number of elements will be derived from the osg uniform.
        Uniform(osg::Uniform* uniform);

        Uniform(osg::Uniform* uniform, Type type, uint elements = 1);
        virtual ~Uniform();

        bool isArray() { return myNumElements != 1; }
        Type getType() { return myType; }
        uint getNumElements() { return myNumElements; }

        //! Setters / getters
        //@{
        void setFloat(float value);
        float getFloat();
        void setFloatElement(float value, uint index);
        float getFloatElement(uint index);
        void setInt(int value);
        int getInt();
        void setIntElement(int value, uint index);
        int getIntElement(uint index);
        void setVector2f(const omega::Vector2f& value);
        omega::Vector2f getVector2f();
        void setVector2fElement(const omega::Vector2f& value, uint index);
        omega::Vector2f getVector2fElement(uint index);
        void setVector3f(const omega::Vector3f& value);
        omega::Vector3f getVector3f();
        void setVector3fElement(const omega::Vector3f& value, uint index);
        omega::Vector3f getVector3fElement(uint element);
        //void setVector4f(const omega::Vector4f& value);
        //omega::Vector4f getVector4f();
        void setColor(const omega::Color& value);
        omega::Color getColor();
        void setColorElement(const omega::Color& value, uint index);
        omega::Color getColorElement(uint index);
        //@}

        //! @internal
        osg::Uniform* getOsgUniform() { return myOsgUniform; }

    private:
        Ref<osg::Uniform> myOsgUniform;
        Type myType;
        uint myNumElements;
    };

    ///////////////////////////////////////////////////////////////////////////
    class CY_API Uniforms: public ReferenceType
    {
    public:
        static osg::Uniform::Type toOsgUniformType(Uniform::Type Type);

    public:
        Uniforms(osg::StateSet* stateset);
        virtual ~Uniforms();

        //! Attaches an existing uniform to this object
        void attachUniform(Uniform*);
        //! Adds a new uniform to this object
        Uniform* addUniform(const String& name, Uniform::Type type);
        //! Adds a new uniform array to this object
        Uniform* addUniformArray(const String& name, Uniform::Type type, uint elements);
        Uniform* getUniform(const String& name);
        void removeAllUniforms();

    private:
        Ref<osg::StateSet> myStateSet;

        typedef Dictionary<String, Ref< Uniform > > UniformDictionary;
        UniformDictionary myUniformDictionary;
    };
};

#endif
