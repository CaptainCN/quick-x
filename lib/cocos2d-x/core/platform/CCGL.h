/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

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

#ifndef __CCGL_H__
#define __CCGL_H__

#include "CCPlatformConfig.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

    #include "GL/glew.h"

    #define CC_GL_DEPTH24_STENCIL8		GL_DEPTH24_STENCIL8

    // These macros are only for making CCTexturePVR.cpp complied without errors since they are not included in GLEW.
    #define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
    #define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
    #define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
    #define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03

#elif CC_TARGET_PLATFORM == CC_PLATFORM_MAC

    #import <OpenGL/gl.h>
    #import <OpenGL/glu.h>
    #import <OpenGL/glext.h>

    #define CC_GL_DEPTH24_STENCIL8      -1


    #define glDeleteVertexArrays            glDeleteVertexArraysAPPLE
    #define glGenVertexArrays               glGenVertexArraysAPPLE
    #define glBindVertexArray               glBindVertexArrayAPPLE

#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

    #define	glClearDepth				glClearDepthf
    #define glDeleteVertexArrays		glDeleteVertexArraysOES
    #define glGenVertexArrays			glGenVertexArraysOES
    #define glBindVertexArray			glBindVertexArrayOES
    #define glMapBuffer					glMapBufferOES
    #define glUnmapBuffer				glUnmapBufferOES


    // GL_GLEXT_PROTOTYPES isn't defined in glplatform.h on android ndk r7 
    // we manually define it here
    // #include <GLES2/gl2platform.h>
    #include <SDL_opengles2.h>
    #ifndef GL_GLEXT_PROTOTYPES
    #define GL_GLEXT_PROTOTYPES 1
    #endif
    #define GL_DEPTH24_STENCIL8			GL_DEPTH24_STENCIL8_OES
    #define GL_WRITE_ONLY				GL_WRITE_ONLY_OES

    // normal process
    // #include <GLES2/gl2.h>
    // #include <GLES2/gl2ext.h>
    // gl2.h doesn't define GLchar on Android
    // typedef char GLchar;
    // android defines GL_BGRA_EXT but not GL_BRGA
    #ifndef GL_BGRA
    #define GL_BGRA  0x80E1
    #endif

    //declare here while define in CCEGLView_android.cpp
    extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT;
    extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT;
    extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT;

    #define glGenVertexArraysOES glGenVertexArraysOESEXT
    #define glBindVertexArrayOES glBindVertexArrayOESEXT
    #define glDeleteVertexArraysOES glDeleteVertexArraysOESEXT

#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS

    #define	glClearDepth				glClearDepthf
    #define glDeleteVertexArrays		glDeleteVertexArraysOES
    #define glGenVertexArrays			glGenVertexArraysOES
    #define glBindVertexArray			glBindVertexArrayOES
    #define glMapBuffer					glMapBufferOES
    #define glUnmapBuffer				glUnmapBufferOES

    #define GL_DEPTH24_STENCIL8			GL_DEPTH24_STENCIL8_OES
    #define GL_WRITE_ONLY				GL_WRITE_ONLY_OES

    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>

#endif



#endif // __CCGL_H__
