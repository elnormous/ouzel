// Copyright (C) 2016 Elviss Strazdins
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
#elif defined(OUZEL_PLATFORM_LINUX)
    #include <GL/gl.h>
    #include <GL/glx.h>
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GL/glext.h>
#endif

namespace ouzel
{
    namespace video
    {
        class RendererOGL;

        class RenderTargetOGL: public RenderTarget
        {
            friend RendererOGL;
        public:
            virtual ~RenderTargetOGL();

            void clean();

            virtual bool init(const Size2& size, bool depthBuffer) override;

            GLuint getFrameBufferId() const { return _framebufferId; }

        protected:
            RenderTargetOGL();

            GLuint _framebufferId = 0;
            GLuint _depthBufferId = 0;
        };
    } // namespace video
} // namespace ouzel
