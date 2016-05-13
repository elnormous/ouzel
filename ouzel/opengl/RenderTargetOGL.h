// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "RenderTarget.h"
#include "Size2.h"
#include "Rectangle.h"

#if defined(OUZEL_PLATFORM_OSX)
    #include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif defined(OUZEL_PLATFORM_ANDROID)
    #include <GLES2/gl2platform.h>
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif defined(OUZEL_PLATFORM_LINUX)
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
#endif

namespace ouzel
{
    namespace graphics
    {
        class RendererOGL;

        class RenderTargetOGL: public RenderTarget
        {
            friend RendererOGL;
        public:
            virtual ~RenderTargetOGL();
            void free() override;

            virtual bool init(const Size2& newSize, bool depthBuffer) override;

            GLuint getFrameBufferId() const { return frameBufferId; }
            const Rectangle& getViewport() const { return viewport; }

        protected:
            RenderTargetOGL();

            GLuint frameBufferId = 0;
            GLuint depthBufferId = 0;
            Rectangle viewport;
        };
    } // namespace graphics
} // namespace ouzel
