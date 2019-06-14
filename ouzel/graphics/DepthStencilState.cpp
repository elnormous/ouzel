// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "DepthStencilState.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        DepthStencilState::DepthStencilState(Renderer& initRenderer):
            resource(*initRenderer.getDevice())
        {
        }

        DepthStencilState::DepthStencilState(Renderer& initRenderer,
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
            resource(*initRenderer.getDevice()),
            depthTest(initDepthTest),
            depthWrite(initDepthWrite),
            compareFunction(initCompareFunction),
            stencilEnabled(initStencilEnabled),
            stencilReadMask(initStencilReadMask),
            stencilWriteMask(initStencilWriteMask),
            frontFaceStencilFailureOperation(initFrontFaceStencilFailureOperation),
            frontFaceStencilDepthFailureOperation(initFrontFaceStencilDepthFailureOperation),
            frontFaceStencilPassOperation(initFrontFaceStencilPassOperation),
            frontFaceStencilCompareFunction(initFrontFaceStencilCompareFunction),
            backFaceStencilFailureOperation(initBackFaceStencilFailureOperation),
            backFaceStencilDepthFailureOperation(initBackFaceStencilDepthFailureOperation),
            backFaceStencilPassOperation(initBackFaceStencilPassOperation),
            backFaceStencilCompareFunction(initBackFaceStencilCompareFunction)
        {
            initRenderer.addCommand(std::unique_ptr<Command>(new InitDepthStencilStateCommand(resource,
                                                                                              initDepthTest,
                                                                                              initDepthWrite,
                                                                                              initCompareFunction,
                                                                                              initStencilEnabled,
                                                                                              initStencilReadMask,
                                                                                              initStencilWriteMask,
                                                                                              initFrontFaceStencilFailureOperation,
                                                                                              initFrontFaceStencilDepthFailureOperation,
                                                                                              initFrontFaceStencilPassOperation,
                                                                                              initFrontFaceStencilCompareFunction,
                                                                                              initBackFaceStencilFailureOperation,
                                                                                              initBackFaceStencilDepthFailureOperation,
                                                                                              initBackFaceStencilPassOperation,
                                                                                              initBackFaceStencilCompareFunction)));
        }
    } // namespace graphics
} // namespace ouzel
