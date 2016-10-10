// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_MACOS
    #include <OpenGL/gl3.h>
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif OUZEL_PLATFORM_ANDROID
    #include <GLES2/gl2platform.h>
    #define GL_GLEXT_PROTOTYPES 1
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif OUZEL_PLATFORM_LINUX
    #define GL_GLEXT_PROTOTYPES 1
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
#elif OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
    #define GL_GLEXT_PROTOTYPES 1
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
            BlendStateOGL();
            virtual ~BlendStateOGL() {}

            GLenum getModeRGB() const { return modeRGB; }
            GLenum getModeAlpha() const { return modeAlpha; }

            GLenum getSourceFactorRGB() const { return sourceFactorRGB; }
            GLenum getDestFactorRGB() const { return destFactorRGB; }
            GLenum getSourceFactorAlpha() const { return sourceFactorAlpha; }
            GLenum getDestFactorAlpha() const { return destFactorAlpha; }
            bool isGLBlendEnabled() const { return glBlendEnabled; }

        protected:
            virtual bool upload() override;

            GLenum modeRGB;
            GLenum modeAlpha;
            GLenum sourceFactorRGB;
            GLenum destFactorRGB;
            GLenum sourceFactorAlpha;
            GLenum destFactorAlpha;
            bool glBlendEnabled;
        };
    } // namespace graphics
} // namespace ouzel
