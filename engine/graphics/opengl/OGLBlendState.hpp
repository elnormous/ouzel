// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLBLENDSTATE_HPP
#define OUZEL_GRAPHICS_OGLBLENDSTATE_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGL.h"

#if OUZEL_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "OGLRenderResource.hpp"
#include "../BlendFactor.hpp"
#include "../BlendOperation.hpp"
#include "../ColorMask.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            class RenderDevice;

            class BlendState final: public RenderResource
            {
            public:
                BlendState(RenderDevice& initRenderDevice,
                           bool enableBlending,
                           BlendFactor colorBlendSource,
                           BlendFactor colorBlendDest,
                           BlendOperation colorOperation,
                           BlendFactor alphaBlendSource,
                           BlendFactor alphaBlendDest,
                           BlendOperation alphaOperation,
                           ColorMask colorMask);

                void reload() final {}

                inline auto getModeRGB() const noexcept { return modeRGB; }
                inline auto getModeAlpha() const noexcept { return modeAlpha; }

                inline auto getSourceFactorRGB() const noexcept { return sourceFactorRGB; }
                inline auto getDestFactorRGB() const noexcept { return destFactorRGB; }
                inline auto getSourceFactorAlpha() const noexcept { return sourceFactorAlpha; }
                inline auto getDestFactorAlpha() const noexcept { return destFactorAlpha; }
                inline auto isBlendEnabled() const noexcept { return blendEnabled; }

                inline auto getRedMask() const noexcept { return redMask; }
                inline auto getGreenMask() const noexcept { return greenMask; }
                inline auto getBlueMask() const noexcept { return blueMask; }
                inline auto getAlphaMask() const noexcept { return alphaMask; }

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
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLBLENDSTATE_HPP
