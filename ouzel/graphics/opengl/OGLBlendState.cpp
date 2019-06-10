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
            static GLenum getBlendFactor(ouzel::graphics::BlendState::Factor blendFactor)
            {
                switch (blendFactor)
                {
                    case ouzel::graphics::BlendState::Factor::ZERO: return GL_ZERO;
                    case ouzel::graphics::BlendState::Factor::ONE: return GL_ONE;
                    case ouzel::graphics::BlendState::Factor::SRC_COLOR: return GL_SRC_COLOR;
                    case ouzel::graphics::BlendState::Factor::INV_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
                    case ouzel::graphics::BlendState::Factor::SRC_ALPHA: return GL_SRC_ALPHA;
                    case ouzel::graphics::BlendState::Factor::INV_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
                    case ouzel::graphics::BlendState::Factor::DEST_ALPHA: return GL_DST_ALPHA;
                    case ouzel::graphics::BlendState::Factor::INV_DEST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
                    case ouzel::graphics::BlendState::Factor::DEST_COLOR: return GL_DST_COLOR;
                    case ouzel::graphics::BlendState::Factor::INV_DEST_COLOR: return GL_ONE_MINUS_DST_COLOR;
                    case ouzel::graphics::BlendState::Factor::SRC_ALPHA_SAT: return GL_SRC_ALPHA_SATURATE;
                    case ouzel::graphics::BlendState::Factor::BLEND_FACTOR: return GL_CONSTANT_COLOR;
                    case ouzel::graphics::BlendState::Factor::INV_BLEND_FACTOR: return GL_ONE_MINUS_CONSTANT_COLOR;
                    default:
                        return GL_NONE;
                }
            }

            static GLenum getBlendOperation(ouzel::graphics::BlendState::Operation blendOperation)
            {
                switch (blendOperation)
                {
                    case ouzel::graphics::BlendState::Operation::ADD: return GL_FUNC_ADD;
                    case ouzel::graphics::BlendState::Operation::SUBTRACT: return GL_FUNC_SUBTRACT;
                    case ouzel::graphics::BlendState::Operation::REV_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
#if !OUZEL_OPENGLES
                    case ouzel::graphics::BlendState::Operation::MIN: return GL_MIN;
                    case ouzel::graphics::BlendState::Operation::MAX: return GL_MAX;
#endif
                    default:
                        return GL_NONE;
                }
            }

            BlendState::BlendState(RenderDevice& renderDevice,
                                   bool enableBlending,
                                   ouzel::graphics::BlendState::Factor colorBlendSource,
                                   ouzel::graphics::BlendState::Factor colorBlendDest,
                                   ouzel::graphics::BlendState::Operation colorOperation,
                                   ouzel::graphics::BlendState::Factor alphaBlendSource,
                                   ouzel::graphics::BlendState::Factor alphaBlendDest,
                                   ouzel::graphics::BlendState::Operation alphaOperation,
                                   uint8_t colorMask):
                RenderResource(renderDevice),
                blendEnabled(enableBlending)
            {
                modeRGB = getBlendOperation(colorOperation);
                modeAlpha = getBlendOperation(alphaOperation);

                sourceFactorRGB = getBlendFactor(colorBlendSource);
                destFactorRGB = getBlendFactor(colorBlendDest);
                sourceFactorAlpha = getBlendFactor(alphaBlendSource);
                destFactorAlpha = getBlendFactor(alphaBlendDest);

                redMask = (colorMask & ouzel::graphics::BlendState::COLOR_MASK_RED) ? GL_TRUE : GL_FALSE;
                greenMask = (colorMask & ouzel::graphics::BlendState::COLOR_MASK_GREEN) ? GL_TRUE : GL_FALSE;
                blueMask = (colorMask & ouzel::graphics::BlendState::COLOR_MASK_BLUE) ? GL_TRUE : GL_FALSE;
                alphaMask = (colorMask & ouzel::graphics::BlendState::COLOR_MASK_ALPHA) ? GL_TRUE : GL_FALSE;
            }
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
