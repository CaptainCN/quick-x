/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "CCShaderCache.h"
#include "CCGLProgram.h"
#include "ccMacros.h"
//kCCShader_Position_uColor
static const GLchar * ccPosition_uColor_frag =
#include "ccShader_Position_uColor_frag.h"
static const GLchar * ccPosition_uColor_vert =
#include "ccShader_Position_uColor_vert.h"
//kCCShader_PositionColor
static const GLchar * ccPositionColor_frag =
#include "ccShader_PositionColor_frag.h"
static const GLchar * ccPositionColor_vert =
#include "ccShader_PositionColor_vert.h"
//kCCShader_PositionTexture
static const GLchar * ccPositionTexture_frag =
#include "ccShader_PositionTexture_frag.h"
static const GLchar * ccPositionTexture_vert =
#include "ccShader_PositionTexture_vert.h"
//kCCShader_PositionTextureA8Color
static const GLchar * ccPositionTextureA8Color_frag =
#include "ccShader_PositionTextureA8Color_frag.h"
static const GLchar * ccPositionTextureA8Color_vert =
#include "ccShader_PositionTextureA8Color_vert.h"
//kCCShader_PositionTextureColor
static const GLchar * ccPositionTextureColor_frag =
#include "ccShader_PositionTextureColor_frag.h"
static const GLchar * ccPositionTextureColor_vert =
#include "ccShader_PositionTextureColor_vert.h"
//kCCShader_PositionTextureGray
static const GLchar * ccPositionTextureGray_frag =
#include "ccShader_PositionTextureGray_frag.h"
static const GLchar * ccPositionTextureGray_vert =
#include "ccShader_PositionTextureColor_vert.h"
//kCCShader_PositionTextureColorAlphaTest
static const GLchar * ccPositionTextureColorAlphaTest_frag =
#include "ccShader_PositionTextureColorAlphaTest_frag.h"
static const GLchar * ccPositionTextureColorAlphaTest_vert =
#include "ccShader_PositionTextureColor_vert.h"
//kCCShader_PositionTexture_uColor
static const GLchar * ccPositionTexture_uColor_frag =
#include "ccShader_PositionTexture_uColor_frag.h"
static const GLchar * ccPositionTexture_uColor_vert =
#include "ccShader_PositionTexture_uColor_vert.h"
//kCCShader_PositionLengthTexureColor
static const GLchar * ccPositionLengthTextureColor_frag =
#include "ccShader_PositionColorLengthTexture_frag.h"
static const GLchar * ccPositionLengthTextureColor_vert =
#include "ccShader_PositionColorLengthTexture_vert.h"

#include "CCRenderState.h"


NS_CC_BEGIN

static SharedPtr<CCShaderCache> _sharedShaderCache;

CCShaderCache* CCShaderCache::sharedShaderCache()
{
    if (!_sharedShaderCache) {
        _sharedShaderCache = new CCShaderCache();
		_sharedShaderCache->init();
    }
    return _sharedShaderCache;
}

void CCShaderCache::purgeSharedShaderCache()
{
	_sharedShaderCache.Reset();
}

CCShaderCache::CCShaderCache()
{

}

CCShaderCache::~CCShaderCache()
{
}



bool CCShaderCache::init()
{
    loadDefaultShaders();
    return true;
}

#define add(type) \
	addRenderStateFromShaderSource(cc##type##_vert, cc##type##_frag)

void CCShaderCache::loadDefaultShaders()
{
	add(PositionTextureColor);
	add(PositionTextureGray);
	add(PositionTextureColorAlphaTest);
	add(PositionColor);
	add(PositionTexture);
	add(PositionTexture_uColor);
	add(PositionTextureA8Color);
	add(Position_uColor);
	add(PositionLengthTextureColor);
}

void CCShaderCache::reloadDefaultShaders()
{
	for (auto s : _shaders)
	{
		s->reset();
		s->loadWithSource();
	}
}

RenderState* CCShaderCache::addRenderStateFromShaderSource(const char* v, const char* f)
{
	SharedPtr<CCGLProgram> shader(new CCGLProgram());
	shader->initWithVertexShaderByteArray(v, f);
	_shaders.push_back(shader);

	SharedPtr<RenderState> mater(new RenderState(shader));
	_materials.push_back(mater);

	return mater.Get();
}

RenderState* CCShaderCache::getRenderState(int key)
{
	return _materials[key];
}

NS_CC_END
