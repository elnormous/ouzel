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
            constexpr GLenum getFunction(CompareFunction compareFunction)
            {
                switch (compareFunction)
                {
                    case CompareFunction::NEVER: return GL_NEVER;
                    case CompareFunction::LESS: return GL_LESS;
                    case CompareFunction::EQUAL: return GL_EQUAL;
                    case CompareFunction::LESS_EQUAL: return GL_LEQUAL;
                    case CompareFunction::GREATER: return GL_GREATER;
                    case CompareFunction::NOT_EQUAL: return GL_NOTEQUAL;
                    case CompareFunction::GREATER_EQUAL: return GL_GEQUAL;
                    case CompareFunction::ALWAYS: return GL_ALWAYS;
                    default: return GL_NEVER;
                }
            }

            constexpr GLenum getOperation(StencilOperation stencilOperation)
            {
                switch (stencilOperation)
                {
                    case StencilOperation::KEEP: return GL_KEEP;
                    case StencilOperation::ZERO: return GL_ZERO;
                    case StencilOperation::REPLACE: return GL_REPLACE;
                    case StencilOperation::INCREMENT_CLAMP: return GL_INCR;
                    case StencilOperation::DECREMENT_CLAMP: return GL_DECR;
                    case StencilOperation::INVERT: return GL_INVERT;
                    case StencilOperation::INCREMENT_WRAP: return GL_INCR_WRAP;
                    case StencilOperation::DECREMENT_WRAP: return GL_DECR_WRAP;
                    default: return GL_KEEP;
                }
            }

            DepthStencilState::DepthStencilState(RenderDevice& initRenderDevice,
                                                 bool initDepthTest,
                                                 bool initDepthWrite,
                                                 CompareFunction initCompareFunction,
                                                 bool initStencilEnabled,
                                                 uint32_t initStencilReadMask,
                                                 uint32_t initStencilWriteMask,
                                                 StencilOperation initFrontFaceStencilFailureOperation,
                                                 StencilOperation initFrontFaceStencilDepthFailureOperation,
                                                 StencilOperation initFrontFaceStencilPassOperation,
                                                 CompareFunction initFrontFaceStencilCompareFunction,
                                                 StencilOperation initBackFaceStencilFailureOperation,
                                                 StencilOperation initBackFaceStencilDepthFailureOperation,
                                                 StencilOperation initBackFaceStencilPassOperation,
                                                 CompareFunction initBackFaceStencilCompareFunction):
                RenderResource(initRenderDevice),
                depthTest(initDepthTest),
                depthMask(initDepthWrite ? GL_TRUE : GL_FALSE),
                compareFunction(getFunction(initCompareFunction)),
                stencilTest(initStencilEnabled),
                stencilReadMask(initStencilReadMask),
                stencilWriteMask(initStencilWriteMask),
                frontFaceFail(getOperation(initFrontFaceStencilFailureOperation)),
                frontFaceDepthFail(getOperation(initFrontFaceStencilDepthFailureOperation)),
                frontFacePass(getOperation(initFrontFaceStencilPassOperation)),
                frontFaceFunction(getFunction(initFrontFaceStencilCompareFunction)),
                backFaceFail(getOperation(initBackFaceStencilFailureOperation)),
                backFaceDepthFail(getOperation(initBackFaceStencilDepthFailureOperation)),
                backFacePass(getOperation(initBackFaceStencilPassOperation)),
                backFaceFunction(getFunction(initBackFaceStencilCompareFunction))
            {
            }
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
