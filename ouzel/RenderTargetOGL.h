// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "RenderTarget.h"
#include "Size2.h"

#if defined(OUZEL_PLATFORM_OSX)
#include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#elif defined(OUZEL_PLATFORM_ANDROID)
#include <GLES2/gl2platform.h>
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#endif

namespace ouzel
{
    class RendererOGL;
    
    class RenderTargetOGL: public RenderTarget
    {
        friend RendererOGL;
    public:
        virtual ~RenderTargetOGL();
        
        bool init(Size2 const& size);
        
        GLuint getFrameBufferId() const { return _framebufferId; }
        GLuint getTextureId() const { return _textureId; }
        
    protected:
        RenderTargetOGL();
        
        GLuint _framebufferId;
        GLuint _textureId;
    };
}
