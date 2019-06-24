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
                    case BlendFactor::ZERO: return GL_ZERO;
                    case BlendFactor::ONE: return GL_ONE;
                    case BlendFactor::SRC_COLOR: return GL_SRC_COLOR;
                    case BlendFactor::INV_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
                    case BlendFactor::SRC_ALPHA: return GL_SRC_ALPHA;
                    case BlendFactor::INV_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
                    case BlendFactor::DEST_ALPHA: return GL_DST_ALPHA;
                    case BlendFactor::INV_DEST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
                    case BlendFactor::DEST_COLOR: return GL_DST_COLOR;
                    case BlendFactor::INV_DEST_COLOR: return GL_ONE_MINUS_DST_COLOR;
                    case BlendFactor::SRC_ALPHA_SAT: return GL_SRC_ALPHA_SATURATE;
                    case BlendFactor::BLEND_FACTOR: return GL_CONSTANT_COLOR;
                    case BlendFactor::INV_BLEND_FACTOR: return GL_ONE_MINUS_CONSTANT_COLOR;
                    default:
                        return GL_NONE;
                }
            }

            static GLenum getBlendOperation(BlendOperation blendOperation)
            {
                switch (blendOperation)
                {
                    case BlendOperation::ADD: return GL_FUNC_ADD;
                    case BlendOperation::SUBTRACT: return GL_FUNC_SUBTRACT;
                    case BlendOperation::REV_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
#if !OUZEL_OPENGLES
                    case BlendOperation::MIN: return GL_MIN;
                    case BlendOperation::MAX: return GL_MAX;
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

                redMask = (colorMask & ColorMask::COLOR_MASK_RED) ? GL_TRUE : GL_FALSE;
                greenMask = (colorMask & ColorMask::COLOR_MASK_GREEN) ? GL_TRUE : GL_FALSE;
                blueMask = (colorMask & ColorMask::COLOR_MASK_BLUE) ? GL_TRUE : GL_FALSE;
                alphaMask = (colorMask & ColorMask::COLOR_MASK_ALPHA) ? GL_TRUE : GL_FALSE;
            }
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
