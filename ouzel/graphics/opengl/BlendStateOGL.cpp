// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendStateOGL.h"
#include "utils/Log.h"

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
                    return GL_ZERO;
            }
        }

        static GLenum getBlendOperation(BlendState::BlendOperation blendOperation)
        {
            switch (blendOperation)
            {
                case BlendState::BlendOperation::ADD: return GL_FUNC_ADD;
                case BlendState::BlendOperation::SUBTRACT: return GL_FUNC_SUBTRACT;
                case BlendState::BlendOperation::REV_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
#if OUZEL_SUPPORTS_OPENGL
                case BlendState::BlendOperation::MIN: return GL_MIN;
                case BlendState::BlendOperation::MAX: return GL_MAX;
#endif
                default:
                    Log(Log::Level::ERR) << "Unsupported blend operation";
                    return GL_FUNC_ADD;
            }
        }

        BlendStateOGL::BlendStateOGL()
        {
        }

        bool BlendStateOGL::upload()
        {
            if (uploadData.dirty)
            {
                modeRGB = getBlendOperation(uploadData.colorOperation);
                modeAlpha = getBlendOperation(uploadData.alphaOperation);

                sourceFactorRGB = getBlendFactor(uploadData.colorBlendSource);
                destFactorRGB = getBlendFactor(uploadData.colorBlendDest);
                sourceFactorAlpha = getBlendFactor(uploadData.alphaBlendDest);
                destFactorAlpha = getBlendFactor(uploadData.alphaBlendDest);

                glBlendEnabled = uploadData.enableBlending;

                uploadData.dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
