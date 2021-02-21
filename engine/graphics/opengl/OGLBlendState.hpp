// Ouzel by Elviss Strazdins

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

namespace ouzel::graphics::opengl
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

        auto getModeRGB() const noexcept { return modeRGB; }
        auto getModeAlpha() const noexcept { return modeAlpha; }

        auto getSourceFactorRGB() const noexcept { return sourceFactorRGB; }
        auto getDestFactorRGB() const noexcept { return destFactorRGB; }
        auto getSourceFactorAlpha() const noexcept { return sourceFactorAlpha; }
        auto getDestFactorAlpha() const noexcept { return destFactorAlpha; }
        auto isBlendEnabled() const noexcept { return blendEnabled; }

        auto getRedMask() const noexcept { return redMask; }
        auto getGreenMask() const noexcept { return greenMask; }
        auto getBlueMask() const noexcept { return blueMask; }
        auto getAlphaMask() const noexcept { return alphaMask; }

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
}

#endif

#endif // OUZEL_GRAPHICS_OGLBLENDSTATE_HPP
