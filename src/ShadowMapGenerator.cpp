/******************************************************************************
 * THE OMEGA LIB PROJECT
 *-----------------------------------------------------------------------------
 * Copyright 2010-2013		Electronic Visualization Laboratory, 
 *							University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-----------------------------------------------------------------------------
 * Copyright (c) 2010-2013, Electronic Visualization Laboratory,  
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
#include <osgShadow/ShadowedScene>
#include <osg/Notify>
#include <osg/ComputeBoundsVisitor>
#include <osg/PolygonOffset>
#include <osg/CullFace>
#include <osg/io_utils>

#include <iostream>
//for debug
#include <osg/LightSource>
#include <osg/PolygonMode>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osgText/Text>

#include "cyclops/ShadowMapGenerator.h"

using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////
ShadowMapGenerator::ShadowMapGenerator():
    _shadowTextureUnit(1),
    _polyOffset(1.0,1.0),
    _textureSize(1024,1024)
{
	_stateset = new osg::StateSet;
    _texture = new osg::Texture2D;
}

///////////////////////////////////////////////////////////////////////////////
ShadowMapGenerator::ShadowMapGenerator(const ShadowMapGenerator& copy, const osg::CopyOp& copyop):
ShadowTechnique(copy,copyop),
    _shadowTextureUnit(copy._shadowTextureUnit),
    _polyOffset(copy._polyOffset),
    _textureSize(copy._textureSize),
	_stateset(copy._stateset)
{
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMapGenerator::setTextureUnit(unsigned int unit)
{
    _shadowTextureUnit = unit;
	if(_textureUnitUniform != NULL)
	{
		_stateset->removeUniform(_textureUnitUniform);
	}
	_textureUnitUniform = new osg::Uniform(
		ostr("shadowTexture%1%", %_shadowTextureUnit).c_str(),
		(int)_shadowTextureUnit);
	_stateset->addUniform(_textureUnitUniform);
    _stateset->setTextureAttributeAndModes(_shadowTextureUnit,_texture.get(),osg::StateAttribute::ON); // | osg::StateAttribute::OVERRIDE);
    _stateset->setTextureMode(_shadowTextureUnit,GL_TEXTURE_GEN_S,osg::StateAttribute::ON);
    _stateset->setTextureMode(_shadowTextureUnit,GL_TEXTURE_GEN_T,osg::StateAttribute::ON);
    _stateset->setTextureMode(_shadowTextureUnit,GL_TEXTURE_GEN_R,osg::StateAttribute::ON);
    _stateset->setTextureMode(_shadowTextureUnit,GL_TEXTURE_GEN_Q,osg::StateAttribute::ON);
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMapGenerator::setPolygonOffset(const osg::Vec2& polyOffset)
{
    _polyOffset = polyOffset;
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMapGenerator::setTextureSize(const osg::Vec2s& textureSize)
{
    _textureSize = textureSize;
    dirty();
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMapGenerator::setLight(osg::Light* light)
{
    _light = light;
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMapGenerator::setLight(osg::LightSource* ls)
{
    _ls = ls;
    _light = _ls->getLight();
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMapGenerator::init()
{
    if (!_shadowedScene) return;

    _texture = new osg::Texture2D;
    _texture->setTextureSize(_textureSize.x(), _textureSize.y());
    _texture->setInternalFormat(GL_DEPTH_COMPONENT);
    _texture->setShadowComparison(true);
    _texture->setShadowTextureMode(osg::Texture2D::LUMINANCE);
    _texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
    _texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);

    // the shadow comparison should fail if object is outside the texture
    _texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
    _texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
    _texture->setBorderColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));

    // set up the render to texture camera.
    {
        // create the camera
        _camera = new osg::Camera;

        _camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF_INHERIT_VIEWPOINT);

        _camera->setCullCallback(new CameraCullCallback(this));

        _camera->setClearMask(GL_DEPTH_BUFFER_BIT);
        //_camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        _camera->setClearColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
        _camera->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);

        // set viewport
        _camera->setViewport(0,0,_textureSize.x(),_textureSize.y());

        // set the camera to render before the main camera.
        _camera->setRenderOrder(osg::Camera::PRE_RENDER);

        // tell the camera to use OpenGL frame buffer object where supported.
        _camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
        //_camera->setRenderTargetImplementation(osg::Camera::SEPERATE_WINDOW);

        // attach the texture and use it as the color buffer.
        _camera->attach(osg::Camera::DEPTH_BUFFER, _texture.get());

        osg::StateSet* stateset = _camera->getOrCreateStateSet();


        // cull front faces so that only backfaces contribute to depth map
        osg::ref_ptr<osg::CullFace> cull_face = new osg::CullFace;
        cull_face->setMode(osg::CullFace::FRONT);
        stateset->setAttribute(cull_face.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        stateset->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

        // negative polygonoffset - move the backface nearer to the eye point so that backfaces
        // shadow themselves
        float factor = -_polyOffset[0];
        float units =  -_polyOffset[1];

        osg::ref_ptr<osg::PolygonOffset> polygon_offset = new osg::PolygonOffset;
        polygon_offset->setFactor(factor);
        polygon_offset->setUnits(units);
        stateset->setAttribute(polygon_offset.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        stateset->setMode(GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    }

    {
        _texgen = new osg::TexGen;
    }

    _dirty = false;
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMapGenerator::update(osg::NodeVisitor& nv)
{
    //_shadowedScene->osg::Group::traverse(nv);
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMapGenerator::cull(osgUtil::CullVisitor& cv)
{
    // record the traversal mask on entry so we can reapply it later.
    unsigned int traversalMask = cv.getTraversalMask();

    osgUtil::RenderStage* orig_rs = cv.getRenderStage();

    // do traversal of shadow recieving scene which does need to be decorated by the shadow map
    //{
    //    cv.pushStateSet(_stateset.get());
    //    _shadowedScene->osg::Group::traverse(cv);
    //    cv.popStateSet();
    //}

    // need to compute view frustum for RTT camera.
    // 1) get the light position
    // 2) get the center and extents of the view frustum
    const osg::Light* selectLight = 0;
    osg::Vec4 lightpos;
    osg::Vec3 lightDir;

    //MR testing giving a specific light
    osgUtil::PositionalStateContainer::AttrMatrixList& aml = orig_rs->getPositionalStateContainer()->getAttrMatrixList();
    for(osgUtil::PositionalStateContainer::AttrMatrixList::iterator itr = aml.begin();
        itr != aml.end();
        ++itr)
    {
        const osg::Light* light = dynamic_cast<const osg::Light*>(itr->first.get());
        if (light)
        {
            if( _light.valid()) {
                if( _light.get() == light )
                    selectLight = light;
                else
                    continue;
            }
            else
                selectLight = light;

            osg::RefMatrix* matrix = itr->second.get();
            if (matrix)
            {
                lightpos = light->getPosition() * (*matrix);
                lightDir = osg::Matrix::transform3x3( light->getDirection(), *matrix );
            }
            else
            {
                lightpos = light->getPosition();
                lightDir = light->getDirection();
            }

        }
    }

    osg::Matrix eyeToWorld;
    eyeToWorld.invert(*cv.getModelViewMatrix());

    lightpos = lightpos * eyeToWorld;
    lightDir = osg::Matrix::transform3x3( lightDir, eyeToWorld );
    lightDir.normalize();

    if (selectLight)
    {
        //std::cout<<"----- VxOSG::ShadowMap selectLight spot cutoff "<<selectLight->getSpotCutoff()<<std::endl;

        float fov = selectLight->getSpotCutoff() * 2;
        if(fov < 180.0f)   // spotlight, then we don't need the bounding box
        {
            osg::Vec3 position(lightpos.x(), lightpos.y(), lightpos.z());
            _camera->setProjectionMatrixAsPerspective(fov, 1.0, 0.1, 1000.0);
            _camera->setViewMatrixAsLookAt(position,position+lightDir,computeOrthogonalVector(lightDir));
        }
        else
        {
            // get the bounds of the model.
            osg::ComputeBoundsVisitor cbbv(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN);
            cbbv.setTraversalMask(getShadowedScene()->getCastsShadowTraversalMask());

            _shadowedScene->osg::Group::traverse(cbbv);

            osg::BoundingBox bb = cbbv.getBoundingBox();

            if (lightpos[3]!=0.0)   // point light
            {
                osg::Vec3 position(lightpos.x(), lightpos.y(), lightpos.z());

                float centerDistance = (position-bb.center()).length();

                float znear = centerDistance-bb.radius();
                float zfar  = centerDistance+bb.radius();
                float zNearRatio = 0.001f;
                if (znear<zfar*zNearRatio) znear = zfar*zNearRatio;

                float top   = (bb.radius()/centerDistance)*znear;
                float right = top;

                _camera->setProjectionMatrixAsFrustum(-right,right,-top,top,znear,zfar);
                _camera->setViewMatrixAsLookAt(position,bb.center(),computeOrthogonalVector(bb.center()-position));
            }
            else    // directional light
            {
                // make an orthographic projection
                osg::Vec3 lightDir(lightpos.x(), lightpos.y(), lightpos.z());
                lightDir.normalize();

                // set the position far away along the light direction
                osg::Vec3 position = bb.center() + lightDir * bb.radius() * 2;

                float centerDistance = (position-bb.center()).length();

                float znear = centerDistance-bb.radius();
                float zfar  = centerDistance+bb.radius();
                float zNearRatio = 0.001f;
                if (znear<zfar*zNearRatio) znear = zfar*zNearRatio;

                float top   = bb.radius();
                float right = top;

                _camera->setProjectionMatrixAsOrtho(-right, right, -top, top, znear, zfar);
                _camera->setViewMatrixAsLookAt(position,bb.center(),computeOrthogonalVector(lightDir));
            }


        }

        cv.setTraversalMask( traversalMask &
            getShadowedScene()->getCastsShadowTraversalMask() );

        // do RTT camera traversal
        _camera->accept(cv);

        _texgen->setMode(osg::TexGen::EYE_LINEAR);

        // compute the matrix which takes a vertex from local coords into tex coords
        // We actually use two matrices one used to define texgen
        // and second that will be used as modelview when appling to OpenGL
        _texgen->setPlanesFromMatrix( _camera->getProjectionMatrix() *
                                      osg::Matrix::translate(1.0,1.0,1.0) *
                                      osg::Matrix::scale(0.5f,0.5f,0.5f) );

        // Place texgen with modelview which removes big offsets (making it float friendly)
        osg::RefMatrix * refMatrix = new osg::RefMatrix
            ( _camera->getInverseViewMatrix() * *cv.getModelViewMatrix() );

        cv.getRenderStage()->getPositionalStateContainer()->
             addPositionedTextureAttribute( _shadowTextureUnit, refMatrix, _texgen.get() );
   } // if(selectLight)


    // reapply the original traversal mask
    cv.setTraversalMask( traversalMask );
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMapGenerator::cleanSceneGraph()
{
}