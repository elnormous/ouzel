// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGLBlendState.hpp"
#include "OGLError.hpp"
#include "OGLRenderDevice.hpp"

namespace ouzel::graphics::opengl
{
    namespace
    {
        constexpr GLenum getBlendFactor(BlendFactor blendFactor)
        {
            switch (blendFactor)
            {
                case BlendFactor::zero: return GL_ZERO;
                case BlendFactor::one: return GL_ONE;
                case BlendFactor::srcColor: return GL_SRC_COLOR;
                case BlendFactor::invSrcColor: return GL_ONE_MINUS_SRC_COLOR;
                case BlendFactor::srcAlpha: return GL_SRC_ALPHA;
                case BlendFactor::invSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
                case BlendFactor::destAlpha: return GL_DST_ALPHA;
                case BlendFactor::invDestAlpha: return GL_ONE_MINUS_DST_ALPHA;
                case BlendFactor::destColor: return GL_DST_COLOR;
                case BlendFactor::invDestColor: return GL_ONE_MINUS_DST_COLOR;
                case BlendFactor::srcAlphaSat: return GL_SRC_ALPHA_SATURATE;
                case BlendFactor::blendFactor: return GL_CONSTANT_COLOR;
                case BlendFactor::invBlendFactor: return GL_ONE_MINUS_CONSTANT_COLOR;
                default: throw Error("Unsupported blend factor");
            }
        }

        constexpr GLenum getBlendOperation(BlendOperation blendOperation)
        {
            switch (blendOperation)
            {
                case BlendOperation::add: return GL_FUNC_ADD;
                case BlendOperation::subtract: return GL_FUNC_SUBTRACT;
                case BlendOperation::reverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
#if !OUZEL_OPENGLES
                case BlendOperation::min: return GL_MIN;
                case BlendOperation::max: return GL_MAX;
#endif
                default: throw Error("Unsupported blend operation");
            }
        }
    }

    BlendState::BlendState(RenderDevice& initRenderDevice,
                           bool enableBlending,
                           BlendFactor colorBlendSource,
                           BlendFactor colorBlendDest,
                           BlendOperation colorOperation,
                           BlendFactor alphaBlendSource,
                           BlendFactor alphaBlendDest,
                           BlendOperation alphaOperation,
                           ColorMask colorMask):
        RenderResource{initRenderDevice},
        blendEnabled{enableBlending}
    {
        modeRGB = getBlendOperation(colorOperation);
        modeAlpha = getBlendOperation(alphaOperation);

        sourceFactorRGB = getBlendFactor(colorBlendSource);
        destFactorRGB = getBlendFactor(colorBlendDest);
        sourceFactorAlpha = getBlendFactor(alphaBlendSource);
        destFactorAlpha = getBlendFactor(alphaBlendDest);

        redMask = ((colorMask & ColorMask::red) == ColorMask::red) ? GL_TRUE : GL_FALSE;
        greenMask = ((colorMask & ColorMask::green) == ColorMask::green) ? GL_TRUE : GL_FALSE;
        blueMask = ((colorMask & ColorMask::blue) == ColorMask::blue) ? GL_TRUE : GL_FALSE;
        alphaMask = ((colorMask & ColorMask::alpha) == ColorMask::alpha) ? GL_TRUE : GL_FALSE;
    }
}

#endif
