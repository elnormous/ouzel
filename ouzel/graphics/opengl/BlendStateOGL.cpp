// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendStateOGL.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        static GLenum getBlendFactor(BlendStateResource::BlendFactor blendFactor)
        {
            switch (blendFactor)
            {
                case BlendStateResource::BlendFactor::ZERO: return GL_ZERO;
                case BlendStateResource::BlendFactor::ONE: return GL_ONE;
                case BlendStateResource::BlendFactor::SRC_COLOR: return GL_SRC_COLOR;
                case BlendStateResource::BlendFactor::INV_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
                case BlendStateResource::BlendFactor::SRC_ALPHA: return GL_SRC_ALPHA;
                case BlendStateResource::BlendFactor::INV_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
                case BlendStateResource::BlendFactor::DEST_ALPHA: return GL_DST_ALPHA;
                case BlendStateResource::BlendFactor::INV_DEST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
                case BlendStateResource::BlendFactor::DEST_COLOR: return GL_DST_COLOR;
                case BlendStateResource::BlendFactor::INV_DEST_COLOR: return GL_ONE_MINUS_DST_COLOR;
                case BlendStateResource::BlendFactor::SRC_ALPHA_SAT: return GL_SRC_ALPHA_SATURATE;
                case BlendStateResource::BlendFactor::BLEND_FACTOR: return GL_CONSTANT_COLOR;
                case BlendStateResource::BlendFactor::INV_BLEND_FACTOR: return GL_ONE_MINUS_CONSTANT_COLOR;
                default:
                    Log(Log::Level::ERR) << "Unsupported blend factor";
                    return GL_ZERO;
            }
        }

        static GLenum getBlendOperation(BlendStateResource::BlendOperation blendOperation)
        {
            switch (blendOperation)
            {
                case BlendStateResource::BlendOperation::ADD: return GL_FUNC_ADD;
                case BlendStateResource::BlendOperation::SUBTRACT: return GL_FUNC_SUBTRACT;
                case BlendStateResource::BlendOperation::REV_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
#if OUZEL_SUPPORTS_OPENGL
                case BlendStateResource::BlendOperation::MIN: return GL_MIN;
                case BlendStateResource::BlendOperation::MAX: return GL_MAX;
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
            if (!BlendStateResource::upload())
            {
                return false;
            }

            modeRGB = getBlendOperation(uploadData.colorOperation);
            modeAlpha = getBlendOperation(uploadData.alphaOperation);

            sourceFactorRGB = getBlendFactor(uploadData.colorBlendSource);
            destFactorRGB = getBlendFactor(uploadData.colorBlendDest);
            sourceFactorAlpha = getBlendFactor(uploadData.alphaBlendDest);
            destFactorAlpha = getBlendFactor(uploadData.alphaBlendDest);

            glBlendEnabled = uploadData.enableBlending;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
