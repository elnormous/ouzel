// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_WINDOWS
    #define NOMINMAX
#endif

#if OUZEL_SUPPORTS_OPENGL
    #define GL_GLEXT_PROTOTYPES 1
    #include "GL/glcorearb.h"
    #include "GL/glext.h"
#elif OUZEL_SUPPORTS_OPENGLES
    #define GL_GLEXT_PROTOTYPES 1
    #include "GLES/gl.h"
    #include "GLES2/gl2.h"
    #include "GLES2/gl2ext.h"
    #include "GLES3/gl3.h"
#endif

#include "graphics/BlendStateResource.h"

namespace ouzel
{
    namespace graphics
    {
        class BlendStateResourceOGL: public BlendStateResource
        {
        public:
            BlendStateResourceOGL();

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
