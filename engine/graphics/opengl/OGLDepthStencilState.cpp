// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGLDepthStencilState.hpp"
#include "OGLError.hpp"
#include "OGLRenderDevice.hpp"

namespace ouzel::graphics::opengl
{
    namespace
    {
        constexpr GLenum getFunction(CompareFunction compareFunction)
        {
            switch (compareFunction)
            {
                case CompareFunction::never: return GL_NEVER;
                case CompareFunction::less: return GL_LESS;
                case CompareFunction::equal: return GL_EQUAL;
                case CompareFunction::lessEqual: return GL_LEQUAL;
                case CompareFunction::greater: return GL_GREATER;
                case CompareFunction::notEqual: return GL_NOTEQUAL;
                case CompareFunction::greaterEqual: return GL_GEQUAL;
                case CompareFunction::always: return GL_ALWAYS;
                default: throw Error{"Unsupported compare function"};
            }
        }

        constexpr GLenum getOperation(StencilOperation stencilOperation)
        {
            switch (stencilOperation)
            {
                case StencilOperation::keep: return GL_KEEP;
                case StencilOperation::zero: return GL_ZERO;
                case StencilOperation::replace: return GL_REPLACE;
                case StencilOperation::incrementClamp: return GL_INCR;
                case StencilOperation::decrementClamp: return GL_DECR;
                case StencilOperation::invert: return GL_INVERT;
                case StencilOperation::incrementWrap: return GL_INCR_WRAP;
                case StencilOperation::decrementWrap: return GL_DECR_WRAP;
                default: throw Error{"Unsupported stencil operation"};
            }
        }
    }

    DepthStencilState::DepthStencilState(RenderDevice& initRenderDevice,
                                         bool initDepthTest,
                                         bool initDepthWrite,
                                         CompareFunction initCompareFunction,
                                         bool initStencilEnabled,
                                         std::uint32_t initStencilReadMask,
                                         std::uint32_t initStencilWriteMask,
                                         StencilOperation initFrontFaceStencilFailureOperation,
                                         StencilOperation initFrontFaceStencilDepthFailureOperation,
                                         StencilOperation initFrontFaceStencilPassOperation,
                                         CompareFunction initFrontFaceStencilCompareFunction,
                                         StencilOperation initBackFaceStencilFailureOperation,
                                         StencilOperation initBackFaceStencilDepthFailureOperation,
                                         StencilOperation initBackFaceStencilPassOperation,
                                         CompareFunction initBackFaceStencilCompareFunction):
        RenderResource{initRenderDevice},
        depthTest{initDepthTest},
        depthMask(initDepthWrite ? GL_TRUE : GL_FALSE),
        compareFunction{getFunction(initCompareFunction)},
        stencilTest{initStencilEnabled},
        stencilReadMask{initStencilReadMask},
        stencilWriteMask{initStencilWriteMask},
        frontFaceFail{getOperation(initFrontFaceStencilFailureOperation)},
        frontFaceDepthFail{getOperation(initFrontFaceStencilDepthFailureOperation)},
        frontFacePass{getOperation(initFrontFaceStencilPassOperation)},
        frontFaceFunction{getFunction(initFrontFaceStencilCompareFunction)},
        backFaceFail{getOperation(initBackFaceStencilFailureOperation)},
        backFaceDepthFail{getOperation(initBackFaceStencilDepthFailureOperation)},
        backFacePass{getOperation(initBackFaceStencilPassOperation)},
        backFaceFunction{getFunction(initBackFaceStencilCompareFunction)}
    {
    }
}

#endif
