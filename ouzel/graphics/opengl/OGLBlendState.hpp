// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLBLENDSTATE_HPP
#define OUZEL_GRAPHICS_OGLBLENDSTATE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "graphics/opengl/OGL.h"

#if OUZEL_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "graphics/opengl/OGLRenderResource.hpp"
#include "graphics/BlendState.hpp"

namespace ouzel
{
    namespace graphics
    {
        class OGLRenderDevice;

        class OGLBlendState final: public OGLRenderResource
        {
        public:
            OGLBlendState(OGLRenderDevice& renderDeviceOGL,
                          bool enableBlending,
                          BlendState::Factor colorBlendSource, BlendState::Factor colorBlendDest,
                          BlendState::Operation colorOperation,
                          BlendState::Factor alphaBlendSource, BlendState::Factor alphaBlendDest,
                          BlendState::Operation alphaOperation,
                          uint8_t colorMask);

            void reload() final {}

            inline GLenum getModeRGB() const { return modeRGB; }
            inline GLenum getModeAlpha() const { return modeAlpha; }

            inline GLenum getSourceFactorRGB() const { return sourceFactorRGB; }
            inline GLenum getDestFactorRGB() const { return destFactorRGB; }
            inline GLenum getSourceFactorAlpha() const { return sourceFactorAlpha; }
            inline GLenum getDestFactorAlpha() const { return destFactorAlpha; }
            inline bool isBlendEnabled() const { return blendEnabled; }

            inline GLboolean getRedMask() const { return redMask; }
            inline GLboolean getGreenMask() const { return greenMask; }
            inline GLboolean getBlueMask() const { return blueMask; }
            inline GLboolean getAlphaMask() const { return alphaMask; }

        private:
            GLenum modeRGB = GL_NONE;
            GLenum modeAlpha = GL_NONE;
            GLenum sourceFactorRGB = GL_NONE;
            GLenum destFactorRGB = GL_NONE;
            GLenum sourceFactorAlpha = GL_NONE;
            GLenum destFactorAlpha = GL_NONE;
            bool blendEnabled = false;

            GLboolean redMask = GL_FALSE;
            GLboolean greenMask = GL_FALSE;
            GLboolean blueMask = GL_FALSE;
            GLboolean alphaMask = GL_FALSE;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLBLENDSTATE_HPP
