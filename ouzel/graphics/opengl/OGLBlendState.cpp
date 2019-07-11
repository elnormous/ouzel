// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGLBlendState.hpp"
#include "OGLRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            static GLenum getBlendFactor(BlendFactor blendFactor)
            {
                switch (blendFactor)
                {
                    case BlendFactor::Zero: return GL_ZERO;
                    case BlendFactor::One: return GL_ONE;
                    case BlendFactor::SrcColor: return GL_SRC_COLOR;
                    case BlendFactor::InvSrcColor: return GL_ONE_MINUS_SRC_COLOR;
                    case BlendFactor::SrcAlpha: return GL_SRC_ALPHA;
                    case BlendFactor::InvSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
                    case BlendFactor::DestAlpha: return GL_DST_ALPHA;
                    case BlendFactor::InvDestAlpha: return GL_ONE_MINUS_DST_ALPHA;
                    case BlendFactor::DestColor: return GL_DST_COLOR;
                    case BlendFactor::InvDestColor: return GL_ONE_MINUS_DST_COLOR;
                    case BlendFactor::SrcAlphaSat: return GL_SRC_ALPHA_SATURATE;
                    case BlendFactor::BlendFactor: return GL_CONSTANT_COLOR;
                    case BlendFactor::InvBlendFactor: return GL_ONE_MINUS_CONSTANT_COLOR;
                    default:
                        return GL_NONE;
                }
            }

            static GLenum getBlendOperation(BlendOperation blendOperation)
            {
                switch (blendOperation)
                {
                    case BlendOperation::Add: return GL_FUNC_ADD;
                    case BlendOperation::Subtract: return GL_FUNC_SUBTRACT;
                    case BlendOperation::RevSubtract: return GL_FUNC_REVERSE_SUBTRACT;
#if !OUZEL_OPENGLES
                    case BlendOperation::Min: return GL_MIN;
                    case BlendOperation::Max: return GL_MAX;
#endif
                    default:
                        return GL_NONE;
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
                                   uint8_t colorMask):
                RenderResource(initRenderDevice),
                blendEnabled(enableBlending)
            {
                modeRGB = getBlendOperation(colorOperation);
                modeAlpha = getBlendOperation(alphaOperation);

                sourceFactorRGB = getBlendFactor(colorBlendSource);
                destFactorRGB = getBlendFactor(colorBlendDest);
                sourceFactorAlpha = getBlendFactor(alphaBlendSource);
                destFactorAlpha = getBlendFactor(alphaBlendDest);

                redMask = (colorMask & ColorMask::Red) ? GL_TRUE : GL_FALSE;
                greenMask = (colorMask & ColorMask::Green) ? GL_TRUE : GL_FALSE;
                blueMask = (colorMask & ColorMask::Blue) ? GL_TRUE : GL_FALSE;
                alphaMask = (colorMask & ColorMask::Alpha) ? GL_TRUE : GL_FALSE;
            }
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
