/******************************************************************************
 * THE OMEGA LIB PROJECT
 *-----------------------------------------------------------------------------
 * Copyright 2010-2015		Electronic Visualization Laboratory, 
 *							University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-----------------------------------------------------------------------------
 * Copyright (c) 2010-2015, Electronic Visualization Laboratory,  
 * osg Copyright (C) 1998-2006 Robert Osfield
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
 *	A modified implementation of the standard Shadow map technique from
 *	OpenSceneGraph
 ******************************************************************************/
#ifndef __CY_SHADOW_MAP_TECHNIQUE__
#define __CY_SHADOW_MAP_TECHNIQUE__

#include "cyclopsConfig.h"
#include <osg/Camera>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osg/LightSource>
#include <osg/Texture3D>

#include <osgShadow/ShadowTechnique>

namespace cyclops {
	class CY_API ShadowMapGenerator : public osgShadow::ShadowTechnique
	{
    public :
        ShadowMapGenerator();
        ShadowMapGenerator(const ShadowMapGenerator& es, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
        META_Object(osgShadow, ShadowMapGenerator);

		osg::Texture2D* getTexture() { return _texture; }

		void setShadowedSceneStateSet(osg::StateSet* ss) { _stateset = ss; }
		osg::StateSet* getShadowedSceneStateSet() { return _stateset; }

        /** Set the texture unit that the shadow texture will be applied on.*/
        void setTextureUnit(unsigned int unit);

        /** Get the texture unit that the shadow texture will be applied on.*/
        unsigned int getTextureUnit() const { return _shadowTextureUnit; }

        /** set the polygon offset used initially */
        void setPolygonOffset(const osg::Vec2& polyOffset);

        /** get the used polygon offset */
        const osg::Vec2& getPolygonOffset() const { return _polyOffset; }

        /** set the size in pixels x / y for the shadow texture.*/
        void setTextureSize(const osg::Vec2s& textureSize);
        const osg::Vec2s& getTextureSize() const { return _textureSize; }

        /** Set the Light that will cast shadows */
        void setLight(osg::Light* light);
        void setLight(osg::LightSource* ls);

        /** initialize the ShadowedScene and local cached data structures.*/
        virtual void init();

        /** run the update traversal of the ShadowedScene and update any loca chached data structures.*/
        virtual void update(osg::NodeVisitor& nv) {}
		virtual void cleanSceneGraph() {}

        /** run the cull traversal of the ShadowedScene and set up the rendering for this ShadowTechnique.*/
        virtual void cull(osgUtil::CullVisitor& cv);

		void setManualRefreshEnabled(bool value) 
		{ myManualRefreshEnabled = value; }
		
		void setDirty() 
		{ myDirty = true; }

		void setSoft(bool value);
		bool isSoft() 
		{ return mySoft; }

		void setSoftShadowParameters(float softnessWidth, float jitteringScale);

	protected:
		static void initJitterTexture();

	protected:
        virtual ~ShadowMapGenerator(void) {};
        osg::ref_ptr<osg::Camera>       _camera;
        osg::ref_ptr<osg::TexGen>       _texgen;
        osg::ref_ptr<osg::Texture2D>    _texture;
        osg::ref_ptr<osg::StateSet>     _stateset;
        osg::ref_ptr<osg::Light>        _light;

        osg::ref_ptr<osg::LightSource>  _ls;

        osg::ref_ptr<osg::Uniform>      _textureUnitUniform;
        unsigned int                    _shadowTextureUnit;
        osg::Vec2                        _polyOffset;
        osg::Vec2s                      _textureSize;

		static Ref<osg::Texture3D> myJitterTexture;
		float myJitteringScale;
		float mySoftnessWidth;
        osg::ref_ptr<osg::Uniform>      myJitteringScaleUniform;
        osg::ref_ptr<osg::Uniform>      mySoftnessWidthUniform;
		unsigned int myJitterTextureUnit;

		bool myManualRefreshEnabled;
		bool myDirty;
		bool mySoft;
    };
}

#endif
