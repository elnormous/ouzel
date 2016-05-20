// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendStateOGL.h"
#include "Utils.h"

namespace ouzel
{
    namespace graphics
    {
        GLenum BlendStateOGL::getBlendFactor(BlendFactor blendFactor)
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
                    log("Unsupported blend factor");
                    return GL_ZERO;
            }
        }

        GLenum BlendStateOGL::getBlendOperation(BlendOperation blendOperation)
        {
            switch (blendOperation)
            {
                case BlendOperation::ADD: return GL_FUNC_ADD;
                case BlendOperation::SUBTRACT: return GL_FUNC_SUBTRACT;
                case BlendOperation::REV_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
#if defined(OUZEL_SUPPORTS_OPENGL)
                case BlendOperation::MIN: return GL_MIN;
                case BlendOperation::MAX: return GL_MAX;
#endif
                default:
                    log("Unsupported blend operation");
                    return GL_FUNC_ADD;
            }
        }

    } // namespace graphics
} // namespace ouzel
