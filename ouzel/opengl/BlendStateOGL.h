// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if defined(OUZEL_PLATFORM_MACOS)
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
#elif defined(OUZEL_PLATFORM_RASPBIAN)
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#endif

#include "graphics/BlendState.h"

namespace ouzel
{
    namespace graphics
    {
        class BlendStateOGL: public BlendState
        {
        public:
            virtual ~BlendStateOGL() {}

            static GLenum getBlendFactor(BlendFactor blendFactor);
            static GLenum getBlendOperation(BlendOperation blendOperation);
        };
    } // namespace graphics
} // namespace ouzel
