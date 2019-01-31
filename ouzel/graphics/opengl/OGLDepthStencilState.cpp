// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGLDepthStencilState.hpp"
#include "OGLRenderDevice.hpp"

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
                default: return GL_NEVER;
            }

            return GL_NEVER;
        }

        static GLenum getOperation(DepthStencilState::StencilOperation stencilOperation)
        {
            switch (stencilOperation)
            {
                case DepthStencilState::StencilOperation::KEEP: return GL_KEEP;
                case DepthStencilState::StencilOperation::ZERO: return GL_ZERO;
                case DepthStencilState::StencilOperation::REPLACE: return GL_REPLACE;
                case DepthStencilState::StencilOperation::INCREMENT_CLAMP: return GL_INCR;
                case DepthStencilState::StencilOperation::DECREMENT_CLAMP: return GL_DECR;
                case DepthStencilState::StencilOperation::INVERT: return GL_INVERT;
                case DepthStencilState::StencilOperation::INCREMENT_WRAP: return GL_INCR_WRAP;
                case DepthStencilState::StencilOperation::DECREMENT_WRAP: return GL_DECR_WRAP;
                default: return GL_KEEP;
            }
        }

        OGLDepthStencilState::OGLDepthStencilState(OGLRenderDevice& renderDeviceOGL,
                                                   bool initDepthTest,
                                                   bool initDepthWrite,
                                                   DepthStencilState::CompareFunction initCompareFunction,
                                                   bool initStencilEnabled,
                                                   uint32_t initStencilReadMask,
                                                   uint32_t initStencilWriteMask,
                                                   const DepthStencilState::StencilDescriptor& initFrontFaceStencil,
                                                   const DepthStencilState::StencilDescriptor& initBackFaceStencil):
            OGLRenderResource(renderDeviceOGL),
            depthTest(initDepthTest),
            depthMask(initDepthWrite ? GL_TRUE : GL_FALSE),
            compareFunction(getFunction(initCompareFunction)),
            stencilTest(initStencilEnabled),
            stencilReadMask(initStencilReadMask),
            stencilWriteMask(initStencilWriteMask),
            frontFaceFail(getOperation(initFrontFaceStencil.failureOperation)),
            frontFaceDepthFail(getOperation(initFrontFaceStencil.depthFailureOperation)),
            frontFacePass(getOperation(initFrontFaceStencil.passOperation)),
            frontFaceFunction(getFunction(initFrontFaceStencil.compareFunction)),
            backFaceFail(getOperation(initBackFaceStencil.failureOperation)),
            backFaceDepthFail(getOperation(initBackFaceStencil.depthFailureOperation)),
            backFacePass(getOperation(initBackFaceStencil.passOperation)),
            backFaceFunction(getFunction(initBackFaceStencil.compareFunction))
        {
        }
    } // namespace graphics
} // namespace ouzel

#endif
