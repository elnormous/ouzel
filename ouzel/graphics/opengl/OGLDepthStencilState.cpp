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
            namespace
            {
                constexpr GLenum getFunction(CompareFunction compareFunction)
                {
                    switch (compareFunction)
                    {
                        case CompareFunction::NeverPass: return GL_NEVER;
                        case CompareFunction::PassIfLess: return GL_LESS;
                        case CompareFunction::PassIfEqual: return GL_EQUAL;
                        case CompareFunction::PassIfLessEqual: return GL_LEQUAL;
                        case CompareFunction::PassIfGreater: return GL_GREATER;
                        case CompareFunction::PassIfNotEqual: return GL_NOTEQUAL;
                        case CompareFunction::PassIfGreaterEqual: return GL_GEQUAL;
                        case CompareFunction::AlwaysPass: return GL_ALWAYS;
                        default: return GL_NEVER;
                    }
                }

                constexpr GLenum getOperation(StencilOperation stencilOperation)
                {
                    switch (stencilOperation)
                    {
                        case StencilOperation::Keep: return GL_KEEP;
                        case StencilOperation::Zero: return GL_ZERO;
                        case StencilOperation::Replace: return GL_REPLACE;
                        case StencilOperation::IncrementClamp: return GL_INCR;
                        case StencilOperation::DecrementClamp: return GL_DECR;
                        case StencilOperation::Invert: return GL_INVERT;
                        case StencilOperation::IncrementWrap: return GL_INCR_WRAP;
                        case StencilOperation::DecrementWrap: return GL_DECR_WRAP;
                        default: return GL_KEEP;
                    }
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
