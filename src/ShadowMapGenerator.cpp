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

// For OsgDrawInformation
#include "omegaOsg/omegaOsg/OsgRenderPass.h"

using namespace cyclops;

Ref<osg::Texture3D> ShadowMapGenerator::myJitterTexture;

///////////////////////////////////////////////////////////////////////////////
ShadowMapGenerator::ShadowMapGenerator():
    _shadowTextureUnit(1),
    _polyOffset(1.0,1.0),
    _textureSize(1024,1024),
    myManualRefreshEnabled(false),
    myDirty(true),
    mySoft(false),
    myJitteringScale(1.0f),
    mySoftnessWidth(0.002f)
{
    _stateset = new osg::StateSet;
    _texture = new osg::Texture2D;
    
    // First shadow map takes care of initializing the jitter texture.
    if(myJitterTexture == NULL) initJitterTexture();
    myJitterTextureUnit = 3;
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

    if(myJitteringScaleUniform != NULL)
    {
        _stateset->removeUniform(myJitteringScaleUniform);
    }
    myJitteringScaleUniform = new osg::Uniform(
        ostr("jitteringScale%1%", %_shadowTextureUnit).c_str(),
        (float)myJitteringScale);
    _stateset->addUniform(myJitteringScaleUniform);

    if(mySoftnessWidthUniform != NULL)
    {
        _stateset->removeUniform(mySoftnessWidthUniform);
    }
    mySoftnessWidthUniform = new osg::Uniform(
        ostr("softnessWidth%1%", %_shadowTextureUnit).c_str(),
        (float)mySoftnessWidth);
    _stateset->addUniform(mySoftnessWidthUniform);

    _stateset->setTextureAttributeAndModes(_shadowTextureUnit,_texture.get(),osg::StateAttribute::ON); // | osg::StateAttribute::OVERRIDE);
    _stateset->setTextureMode(_shadowTextureUnit,GL_TEXTURE_GEN_S,osg::StateAttribute::ON);
    _stateset->setTextureMode(_shadowTextureUnit,GL_TEXTURE_GEN_T,osg::StateAttribute::ON);
    _stateset->setTextureMode(_shadowTextureUnit,GL_TEXTURE_GEN_R,osg::StateAttribute::ON);
    _stateset->setTextureMode(_shadowTextureUnit,GL_TEXTURE_GEN_Q,osg::StateAttribute::ON);

    if(mySoft)
    {
        _stateset->setTextureAttributeAndModes(myJitterTextureUnit, myJitterTexture, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        _stateset->setTextureMode(myJitterTextureUnit,GL_TEXTURE_GEN_S,osg::StateAttribute::ON);
        _stateset->setTextureMode(myJitterTextureUnit,GL_TEXTURE_GEN_T,osg::StateAttribute::ON);
        _stateset->setTextureMode(myJitterTextureUnit,GL_TEXTURE_GEN_R,osg::StateAttribute::ON);
        osg::Uniform* jitterTextureSampler = new osg::Uniform("osgShadow_jitterTexture",(int)myJitterTextureUnit);
        _stateset->addUniform(jitterTextureSampler);
    }
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMapGenerator::setSoftShadowParameters(float softnessWidth, float jitteringScale)
{
    mySoftnessWidth = softnessWidth;
    myJitteringScale = jitteringScale;
    if(mySoftnessWidthUniform != NULL && myJitteringScaleUniform != NULL)
    {
        mySoftnessWidthUniform->set(mySoftnessWidth);
        myJitteringScaleUniform->set(myJitteringScale);
    }
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
        // CHANGE 1/9/2015 - changed offsets to positive since shadows seem
        // to look much better this way. Keep an eye on this if you find cases
        // of bad looking shadows.
        float factor = 1;// -_polyOffset[0];
        float units = 0;// -_polyOffset[1];

        osg::ref_ptr<osg::PolygonOffset> polygon_offset = new osg::PolygonOffset;
        polygon_offset->setFactor(factor);
        polygon_offset->setUnits(units);
        stateset->setAttribute(polygon_offset.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        stateset->setMode(GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        stateset->setAttributeAndModes(new osg::Program(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
    }

    _texgen = new osg::TexGen;
    _dirty = false;

    // Force-reset the shadow texture unit, to register the new texture.
    setTextureUnit(_shadowTextureUnit);

    // Setting the dirty flag to true makes sure we regenerate the shadow map
    // after initializing it.
    myDirty = true;
}

///////////////////////////////////////////////////////////////////////////////
void ShadowMapGenerator::cull(osgUtil::CullVisitor& cv)
{
    bool needShadowRefresh = true;
    // Here we process a couple of quick exit conditions:
    omegaOsg::OsgDrawInformation* odi = 
        dynamic_cast<omegaOsg::OsgDrawInformation*>(cv.getRenderStage()->getCamera()->getUserData());
    if(odi != NULL)
    {
        // Condition 1: do RTT camera traversal only if depth partitoning is off or
        // if we are drawing the near partition (far partition will never have 
        // shadow maps).
        if(odi->depthPartitionMode == omegaOsg::OsgDrawInformation::DepthPartitionFarOnly) 
        {
            needShadowRefresh = false;
        }

        // Condition 3: only do RTT traversal on one eye (cyclops or the first eye in
        // stereo mode)
        if(odi->context->eye == DrawContext::EyeRight)
        {
            needShadowRefresh = false;
        }
    }

    // Condition 3: do RTT camera traversal only if shadow map needs to be refreshed
    if(myManualRefreshEnabled && !myDirty)
    {
        needShadowRefresh = false;
    }

    // record the traversal mask on entry so we can reapply it later.
    unsigned int traversalMask = cv.getTraversalMask();

    osgUtil::RenderStage* orig_rs = cv.getCurrentRenderStage();

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

        if(needShadowRefresh)
        {
            myDirty = false;
            // NOTE: Camera accept will use the ShadowTechnique::CameraCallback
            // registered during init to traverse the rest of the scene attached
            // to the ShadowedScene object
            _camera->accept(cv);
        }

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
        // reapply the original traversal mask
        cv.setTraversalMask( traversalMask );

    }
}

///////////////////////////////////////////////////////////////////////////////
// Implementation from Chapter 17, Efficient Soft-Edged Shadows Using Pixel 
// Shader Branching, Yury Uralsky. GPU Gems 2, Matt Pharr ed. Addison-Wesley.
// Creates a 3D texture containing jittering data used in the shader to take 
// samples of the shadow map.
void ShadowMapGenerator::initJitterTexture()
{
    // create a 3D texture with hw mipmapping
    myJitterTexture = new osg::Texture3D;
    myJitterTexture->setFilter(osg::Texture3D::MIN_FILTER,osg::Texture3D::NEAREST);
    myJitterTexture->setFilter(osg::Texture3D::MAG_FILTER,osg::Texture3D::NEAREST);
    myJitterTexture->setWrap(osg::Texture3D::WRAP_S,osg::Texture3D::REPEAT);
    myJitterTexture->setWrap(osg::Texture3D::WRAP_T,osg::Texture3D::REPEAT);
    myJitterTexture->setWrap(osg::Texture3D::WRAP_R,osg::Texture3D::REPEAT);
    myJitterTexture->setUseHardwareMipMapGeneration(true);

    const unsigned int size = 16;
    const unsigned int gridW =  8;
    const unsigned int gridH =  8;
    unsigned int R = (gridW * gridH / 2);
    myJitterTexture->setTextureSize(size, size, R);

    // then create the 3d image to fill with jittering data
    osg::Image* image3D = new osg::Image;
    unsigned char *data3D = new unsigned char[size * size * R * 4];

    for ( unsigned int s = 0; s < size; ++s )
    {
        for ( unsigned int t = 0; t < size; ++t )
        {
            float v[4], d[4];

            for ( unsigned int r = 0; r < R; ++r )
            {
                const int x = r % ( gridW / 2 );
                const int y = ( gridH - 1 ) - ( r / (gridW / 2) );

                // Generate points on a  regular gridW x gridH rectangular
                // grid.   We  multiply  x   by  2  because,  we  treat  2
                // consecutive x  each loop iteration.  Add 0.5f  to be in
                // the center of the pixel. x, y belongs to [ 0.0, 1.0 ].
                v[0] = float( x * 2     + 0.5f ) / gridW;
                v[1] = float( y         + 0.5f ) / gridH;
                v[2] = float( x * 2 + 1 + 0.5f ) / gridW;
                v[3] = v[1];

                // Jitter positions. ( 0.5f / w ) == ( 1.0f / 2*w )
                v[0] += ((float)rand() * 2.f / RAND_MAX - 1.f) * ( 0.5f / gridW );
                v[1] += ((float)rand() * 2.f / RAND_MAX - 1.f) * ( 0.5f / gridH );
                v[2] += ((float)rand() * 2.f / RAND_MAX - 1.f) * ( 0.5f / gridW );
                v[3] += ((float)rand() * 2.f / RAND_MAX - 1.f) * ( 0.5f / gridH );

                // Warp to disk; values in [-1,1]
                d[0] = sqrtf( v[1] ) * cosf( 2.f * 3.1415926f * v[0] );
                d[1] = sqrtf( v[1] ) * sinf( 2.f * 3.1415926f * v[0] );
                d[2] = sqrtf( v[3] ) * cosf( 2.f * 3.1415926f * v[2] );
                d[3] = sqrtf( v[3] ) * sinf( 2.f * 3.1415926f * v[2] );

                // store d into unsigned values [0,255]
                const unsigned int tmp = ( (r * size * size) + (t * size) + s ) * 4;
                data3D[ tmp + 0 ] = (unsigned char)( ( 1.f + d[0] ) * 127  );
                data3D[ tmp + 1 ] = (unsigned char)( ( 1.f + d[1] ) * 127  );
                data3D[ tmp + 2 ] = (unsigned char)( ( 1.f + d[2] ) * 127  );
                data3D[ tmp + 3 ] = (unsigned char)( ( 1.f + d[3] ) * 127  );

            }
        }
    }

    // the GPU Gem implementation uses a NV specific internal texture format (GL_SIGNED_RGBA_NV)
    // In order to make it more generic, we use GL_RGBA4 which should be cross platform.
    #ifdef GL_RGBA4
    GLenum internalTextureFormat = GL_RGBA4;
    #else
    // OpenGLES 1.1 doesn't define GL_RGBA4, so we'll just assume RGBA
    GLenum internalTextureFormat = GL_RGBA;
    #endif
    image3D->setImage(size, size, R, internalTextureFormat, GL_RGBA, GL_UNSIGNED_BYTE, data3D, osg::Image::USE_NEW_DELETE);

    myJitterTexture->setImage(image3D);

}

///////////////////////////////////////////////////////////////////////////////
void ShadowMapGenerator::setSoft(bool value)
{
    if(mySoft != value)
    {
        mySoft = value;
        dirty();
    }
}