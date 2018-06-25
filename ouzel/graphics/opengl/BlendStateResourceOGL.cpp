// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "BlendStateResourceOGL.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        static GLenum getBlendFactor(BlendState::Factor blendFactor)
        {
            switch (blendFactor)
            {
                case BlendState::Factor::ZERO: return GL_ZERO;
                case BlendState::Factor::ONE: return GL_ONE;
                case BlendState::Factor::SRC_COLOR: return GL_SRC_COLOR;
                case BlendState::Factor::INV_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
                case BlendState::Factor::SRC_ALPHA: return GL_SRC_ALPHA;
                case BlendState::Factor::INV_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
                case BlendState::Factor::DEST_ALPHA: return GL_DST_ALPHA;
                case BlendState::Factor::INV_DEST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
                case BlendState::Factor::DEST_COLOR: return GL_DST_COLOR;
                case BlendState::Factor::INV_DEST_COLOR: return GL_ONE_MINUS_DST_COLOR;
                case BlendState::Factor::SRC_ALPHA_SAT: return GL_SRC_ALPHA_SATURATE;
                case BlendState::Factor::BLEND_FACTOR: return GL_CONSTANT_COLOR;
                case BlendState::Factor::INV_BLEND_FACTOR: return GL_ONE_MINUS_CONSTANT_COLOR;
                default:
                    return GL_NONE;
            }
        }

        static GLenum getBlendOperation(BlendState::Operation blendOperation)
        {
            switch (blendOperation)
            {
                case BlendState::Operation::ADD: return GL_FUNC_ADD;
                case BlendState::Operation::SUBTRACT: return GL_FUNC_SUBTRACT;
                case BlendState::Operation::REV_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
#if !OUZEL_SUPPORTS_OPENGLES
                case BlendState::Operation::MIN: return GL_MIN;
                case BlendState::Operation::MAX: return GL_MAX;
#endif
                default:
                    return GL_NONE;
            }
        }

        BlendStateResourceOGL::BlendStateResourceOGL()
        {
        }

        void BlendStateResourceOGL::init(bool newEnableBlending,
                                         BlendState::Factor newColorBlendSource, BlendState::Factor newColorBlendDest,
                                         BlendState::Operation newColorOperation,
                                         BlendState::Factor newAlphaBlendSource, BlendState::Factor newAlphaBlendDest,
                                         BlendState::Operation newAlphaOperation,
                                         uint8_t newColorMask)
        {
            BlendStateResource::init(newEnableBlending,
                                     newColorBlendSource, newColorBlendDest,
                                     newColorOperation,
                                     newAlphaBlendSource, newAlphaBlendDest,
                                     newAlphaOperation,
                                     newColorMask);

            modeRGB = getBlendOperation(colorOperation);
            modeAlpha = getBlendOperation(alphaOperation);

            sourceFactorRGB = getBlendFactor(colorBlendSource);
            destFactorRGB = getBlendFactor(colorBlendDest);
            sourceFactorAlpha = getBlendFactor(alphaBlendDest);
            destFactorAlpha = getBlendFactor(alphaBlendDest);

            glBlendEnabled = enableBlending;

            redMask = (colorMask & BlendState::COLOR_MASK_RED) ? GL_TRUE : GL_FALSE;
            greenMask = (colorMask & BlendState::COLOR_MASK_GREEN) ? GL_TRUE : GL_FALSE;
            blueMask = (colorMask & BlendState::COLOR_MASK_BLUE) ? GL_TRUE : GL_FALSE;
            alphaMask = (colorMask & BlendState::COLOR_MASK_ALPHA) ? GL_TRUE : GL_FALSE;
        }
    } // namespace graphics
} // namespace ouzel

#endif
