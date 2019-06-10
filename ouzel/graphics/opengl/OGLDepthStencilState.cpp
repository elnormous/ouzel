// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGLDepthStencilState.hpp"
#include "OGLRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            static GLenum getFunction(ouzel::graphics::DepthStencilState::CompareFunction compareFunction)
            {
                switch (compareFunction)
                {
                    case ouzel::graphics::DepthStencilState::CompareFunction::NEVER: return GL_NEVER;
                    case ouzel::graphics::DepthStencilState::CompareFunction::LESS: return GL_LESS;
                    case ouzel::graphics::DepthStencilState::CompareFunction::EQUAL: return GL_EQUAL;
                    case ouzel::graphics::DepthStencilState::CompareFunction::LESS_EQUAL: return GL_LEQUAL;
                    case ouzel::graphics::DepthStencilState::CompareFunction::GREATER: return GL_GREATER;
                    case ouzel::graphics::DepthStencilState::CompareFunction::NOT_EQUAL: return GL_NOTEQUAL;
                    case ouzel::graphics::DepthStencilState::CompareFunction::GREATER_EQUAL: return GL_GEQUAL;
                    case ouzel::graphics::DepthStencilState::CompareFunction::ALWAYS: return GL_ALWAYS;
                    default: return GL_NEVER;
                }
            }

            static GLenum getOperation(ouzel::graphics::DepthStencilState::StencilOperation stencilOperation)
            {
                switch (stencilOperation)
                {
                    case ouzel::graphics::DepthStencilState::StencilOperation::KEEP: return GL_KEEP;
                    case ouzel::graphics::DepthStencilState::StencilOperation::ZERO: return GL_ZERO;
                    case ouzel::graphics::DepthStencilState::StencilOperation::REPLACE: return GL_REPLACE;
                    case ouzel::graphics::DepthStencilState::StencilOperation::INCREMENT_CLAMP: return GL_INCR;
                    case ouzel::graphics::DepthStencilState::StencilOperation::DECREMENT_CLAMP: return GL_DECR;
                    case ouzel::graphics::DepthStencilState::StencilOperation::INVERT: return GL_INVERT;
                    case ouzel::graphics::DepthStencilState::StencilOperation::INCREMENT_WRAP: return GL_INCR_WRAP;
                    case ouzel::graphics::DepthStencilState::StencilOperation::DECREMENT_WRAP: return GL_DECR_WRAP;
                    default: return GL_KEEP;
                }
            }

            DepthStencilState::DepthStencilState(RenderDevice& renderDevice,
                                                 bool initDepthTest,
                                                 bool initDepthWrite,
                                                 ouzel::graphics::DepthStencilState::CompareFunction initCompareFunction,
                                                 bool initStencilEnabled,
                                                 uint32_t initStencilReadMask,
                                                 uint32_t initStencilWriteMask,
                                                 const ouzel::graphics::DepthStencilState::StencilDescriptor& initFrontFaceStencil,
                                                 const ouzel::graphics::DepthStencilState::StencilDescriptor& initBackFaceStencil):
                RenderResource(renderDevice),
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
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
