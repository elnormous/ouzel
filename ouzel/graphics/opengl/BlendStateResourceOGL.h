// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#if OUZEL_SUPPORTS_OPENGLES
    #define GL_GLEXT_PROTOTYPES 1
    #include "GLES/gl.h"
    #include "GLES2/gl2.h"
    #include "GLES2/gl2ext.h"
    #include "GLES3/gl3.h"
#else
    #define GL_GLEXT_PROTOTYPES 1
    #include "GL/glcorearb.h"
    #include "GL/glext.h"
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

            GLboolean getRedMask() const { return redMask; }
            GLboolean getGreenMask() const { return greenMask; }
            GLboolean getBlueMask() const { return blueMask; }
            GLboolean getAlphaMask() const { return alphaMask; }

        protected:
            virtual bool upload() override;

            GLenum modeRGB = GL_NONE;
            GLenum modeAlpha = GL_NONE;
            GLenum sourceFactorRGB = GL_NONE;
            GLenum destFactorRGB = GL_NONE;
            GLenum sourceFactorAlpha = GL_NONE;
            GLenum destFactorAlpha = GL_NONE;
            bool glBlendEnabled = false;

            GLboolean redMask = GL_FALSE;
            GLboolean greenMask = GL_FALSE;
            GLboolean blueMask = GL_FALSE;
            GLboolean alphaMask = GL_FALSE;
        };
    } // namespace graphics
} // namespace ouzel

#endif
