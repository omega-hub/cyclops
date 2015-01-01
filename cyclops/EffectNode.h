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
 *	An osg node handling visual effects applied to entities. This class is for
 *  internal use only.
 ******************************************************************************/
#ifndef __CY_EFFECT__
#define __CY_EFFECT__

#include "cyclopsConfig.h"
#include "Material.h"

#include <osg/Material>
#include <osg/StateSet>
#include <osgUtil/CullVisitor>

#define OMEGA_NO_GL_HEADERS
#include <omega.h>
#include <omegaOsg/omegaOsg.h>
#include <omegaToolkit.h>

namespace cyclops {
    using namespace omega;
    using namespace omegaOsg;
    class SceneManager;
    class ShaderManager;

    ///////////////////////////////////////////////////////////////////////////
    class CY_API EffectNode: public osg::Group
    {
    public:

    public:
        EffectNode() {}
        EffectNode(SceneManager* sm);
        virtual ~EffectNode();

        EffectNode(const EffectNode& copy, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY) : osg::Group(copy, op) {	}

        // Effect class info
        META_Object(cyclops, EffectNode);

        String getDefinition();
        void setDefinition(const String& definition);

        //TODO: Is this needed?
        //SceneManager* getSceneManager() { return mySceneManager; }

        Material* getMaterial(unsigned int index);
        void addMaterial(Material* mat);
        int getMaterialCount();
        void clearMaterials();

        //! @internal sets the shader manager used by this effect to manage
        //! material shaders
        void setShaderManager(ShaderManager* sm);

        //! @internal
        virtual void traverse(osg::NodeVisitor& nv);

    private:
        Vector< Ref<Material> > myMaterials;

        String myDefinition;
        SceneManager* mySceneManager;
        ShaderManager* myShaderManager;
    };
    
    ///////////////////////////////////////////////////////////////////////////
    inline int EffectNode::getMaterialCount()
    { return myMaterials.size(); }

    ///////////////////////////////////////////////////////////////////////////
    inline String EffectNode::getDefinition() 
    { return myDefinition; }
};

#endif
