// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.hpp"

#if OUZEL_SUPPORTS_OPENGL

#include "BlendStateInterfaceOGL.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        static GLenum getBlendFactor(BlendState::BlendFactor blendFactor)
        {
            switch (blendFactor)
            {
                case BlendState::BlendFactor::ZERO: return GL_ZERO;
                case BlendState::BlendFactor::ONE: return GL_ONE;
                case BlendState::BlendFactor::SRC_COLOR: return GL_SRC_COLOR;
                case BlendState::BlendFactor::INV_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
                case BlendState::BlendFactor::SRC_ALPHA: return GL_SRC_ALPHA;
                case BlendState::BlendFactor::INV_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
                case BlendState::BlendFactor::DEST_ALPHA: return GL_DST_ALPHA;
                case BlendState::BlendFactor::INV_DEST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
                case BlendState::BlendFactor::DEST_COLOR: return GL_DST_COLOR;
                case BlendState::BlendFactor::INV_DEST_COLOR: return GL_ONE_MINUS_DST_COLOR;
                case BlendState::BlendFactor::SRC_ALPHA_SAT: return GL_SRC_ALPHA_SATURATE;
                case BlendState::BlendFactor::BLEND_FACTOR: return GL_CONSTANT_COLOR;
                case BlendState::BlendFactor::INV_BLEND_FACTOR: return GL_ONE_MINUS_CONSTANT_COLOR;
                default:
                    Log(Log::Level::ERR) << "Unsupported blend factor";
                    return GL_NONE;
            }
        }

        static GLenum getBlendOperation(BlendState::BlendOperation blendOperation)
        {
            switch (blendOperation)
            {
                case BlendState::BlendOperation::ADD: return GL_FUNC_ADD;
                case BlendState::BlendOperation::SUBTRACT: return GL_FUNC_SUBTRACT;
                case BlendState::BlendOperation::REV_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
#if !OUZEL_SUPPORTS_OPENGLES
                case BlendState::BlendOperation::MIN: return GL_MIN;
                case BlendState::BlendOperation::MAX: return GL_MAX;
#endif
                default:
                    Log(Log::Level::ERR) << "Unsupported blend operation";
                    return GL_NONE;
            }
        }

        BlendStateInterfaceOGL::BlendStateInterfaceOGL()
        {
        }

        bool BlendStateInterfaceOGL::init(bool newEnableBlending,
                                         BlendState::BlendFactor newColorBlendSource, BlendState::BlendFactor newColorBlendDest,
                                         BlendState::BlendOperation newColorOperation,
                                         BlendState::BlendFactor newAlphaBlendSource, BlendState::BlendFactor newAlphaBlendDest,
                                         BlendState::BlendOperation newAlphaOperation,
                                         uint8_t newColorMask)
        {
            if (!BlendStateInterface::init(newEnableBlending,
                                          newColorBlendSource, newColorBlendDest,
                                          newColorOperation,
                                          newAlphaBlendSource, newAlphaBlendDest,
                                          newAlphaOperation,
                                          newColorMask))
            {
                return false;
            }

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

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
