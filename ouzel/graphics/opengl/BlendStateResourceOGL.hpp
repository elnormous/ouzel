// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#if OUZEL_SUPPORTS_OPENGLES
#  define GL_GLEXT_PROTOTYPES 1
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  define GL_GLEXT_PROTOTYPES 1
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "graphics/opengl/RenderResourceOGL.hpp"
#include "graphics/BlendState.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;

        class BlendStateResourceOGL: public RenderResourceOGL
        {
        public:
            BlendStateResourceOGL(RenderDeviceOGL& renderDeviceOGL);

            void init(bool newEnableBlending,
                      BlendState::Factor newColorBlendSource, BlendState::Factor newColorBlendDest,
                      BlendState::Operation newColorOperation,
                      BlendState::Factor newAlphaBlendSource, BlendState::Factor newAlphaBlendDest,
                      BlendState::Operation newAlphaOperation,
                      uint8_t newColorMask);

            virtual void reload() override {}

            inline GLenum getModeRGB() const { return modeRGB; }
            inline GLenum getModeAlpha() const { return modeAlpha; }

            inline GLenum getSourceFactorRGB() const { return sourceFactorRGB; }
            inline GLenum getDestFactorRGB() const { return destFactorRGB; }
            inline GLenum getSourceFactorAlpha() const { return sourceFactorAlpha; }
            inline GLenum getDestFactorAlpha() const { return destFactorAlpha; }
            inline bool isGLBlendEnabled() const { return glBlendEnabled; }

            inline GLboolean getRedMask() const { return redMask; }
            inline GLboolean getGreenMask() const { return greenMask; }
            inline GLboolean getBlueMask() const { return blueMask; }
            inline GLboolean getAlphaMask() const { return alphaMask; }

        private:
            BlendState::Factor colorBlendSource = BlendState::Factor::ONE;
            BlendState::Factor colorBlendDest = BlendState::Factor::ZERO;
            BlendState::Operation colorOperation = BlendState::Operation::ADD;
            BlendState::Factor alphaBlendSource = BlendState::Factor::ONE;
            BlendState::Factor alphaBlendDest = BlendState::Factor::ZERO;
            BlendState::Operation alphaOperation = BlendState::Operation::ADD;
            uint8_t colorMask = BlendState::COLOR_MASK_ALL;
            bool enableBlending = false;

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
