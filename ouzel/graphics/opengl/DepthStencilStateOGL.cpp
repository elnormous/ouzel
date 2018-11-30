// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "DepthStencilStateOGL.hpp"
#include "RenderDeviceOGL.hpp"

namespace ouzel
{
    namespace graphics
    {
        static GLenum getFunction(DepthStencilState::CompareFunction compareFunction)
        {
            switch (compareFunction)
            {
                case DepthStencilState::CompareFunction::NEVER: return GL_NEVER;
                case DepthStencilState::CompareFunction::LESS: return GL_LESS;
                case DepthStencilState::CompareFunction::EQUAL: return GL_EQUAL;
                case DepthStencilState::CompareFunction::LESS_EQUAL: return GL_LEQUAL;
                case DepthStencilState::CompareFunction::GREATER: return GL_GREATER;
                case DepthStencilState::CompareFunction::NOT_EQUAL: return GL_NOTEQUAL;
                case DepthStencilState::CompareFunction::GREATER_EQUAL: return GL_GEQUAL;
                case DepthStencilState::CompareFunction::ALWAYS: return GL_ALWAYS;
            }

            return GL_NEVER;
        }

        DepthStencilStateOGL::DepthStencilStateOGL(RenderDeviceOGL& renderDeviceOGL,
                                                   bool initDepthTest,
                                                   bool initDepthWrite,
                                                   DepthStencilState::CompareFunction initCompareFunction):
            RenderResourceOGL(renderDeviceOGL),
            depthTest(initDepthTest),
            depthMask(initDepthWrite ? GL_TRUE : GL_FALSE),
            compareFunction(getFunction(initCompareFunction))
        {
        }
    } // namespace graphics
} // namespace ouzel

#endif
