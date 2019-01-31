// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "DepthStencilState.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        DepthStencilState::DepthStencilState(Renderer& initRenderer):
            renderer(&initRenderer),
            resource(renderer->getDevice()->getResourceId())
        {
        }

        DepthStencilState::DepthStencilState(Renderer& initRenderer,
                                             bool initDepthTest,
                                             bool initDepthWrite,
                                             CompareFunction initCompareFunction,
                                             bool initStencilEnabled,
                                             uint32_t initStencilReadMask,
                                             uint32_t initStencilWriteMask,
                                             const StencilDescriptor& initFrontFaceStencil,
                                             const StencilDescriptor& initBackFaceStencil):
            renderer(&initRenderer),
            resource(renderer->getDevice()->getResourceId()),
            depthTest(initDepthTest),
            depthWrite(initDepthWrite),
            compareFunction(initCompareFunction),
            stencilEnabled(initStencilEnabled),
            stencilReadMask(initStencilReadMask),
            stencilWriteMask(initStencilWriteMask),
            frontFaceStencil(initFrontFaceStencil),
            backFaceStencil(initBackFaceStencil)
        {
            if (renderer && resource)
                renderer->addCommand(std::unique_ptr<Command>(new InitDepthStencilStateCommand(resource,
                                                                                               initDepthTest,
                                                                                               initDepthWrite,
                                                                                               initCompareFunction,
                                                                                               initStencilEnabled,
                                                                                               initStencilReadMask,
                                                                                               initStencilWriteMask,
                                                                                               initFrontFaceStencil,
                                                                                               initBackFaceStencil)));
        }

        DepthStencilState::~DepthStencilState()
        {
            if (renderer && resource)
            {
                renderer->addCommand(std::unique_ptr<Command>(new DeleteResourceCommand(resource)));
                RenderDevice* renderDevice = renderer->getDevice();
                renderDevice->deleteResourceId(resource);
            }
        }

        void DepthStencilState::init(bool newDepthTest,
                                     bool newDepthWrite,
                                     CompareFunction newCompareFunction,
                                     bool initStencilEnabled,
                                     uint32_t newStencilReadMask,
                                     uint32_t newStencilWriteMask,
                                     StencilDescriptor initFrontFaceStencil,
                                     StencilDescriptor initBackFaceStencil)
        {
            depthTest = newDepthTest;
            depthWrite = newDepthWrite;
            compareFunction = newCompareFunction;
            stencilEnabled = initStencilEnabled;
            stencilReadMask = newStencilReadMask;
            stencilWriteMask = newStencilWriteMask;
            frontFaceStencil = initFrontFaceStencil;
            backFaceStencil = initBackFaceStencil;

            if (renderer && resource)
                renderer->addCommand(std::unique_ptr<Command>(new InitDepthStencilStateCommand(resource,
                                                                                               newDepthTest,
                                                                                               newDepthWrite,
                                                                                               newCompareFunction,
                                                                                               initStencilEnabled,
                                                                                               newStencilReadMask,
                                                                                               newStencilWriteMask,
                                                                                               initFrontFaceStencil,
                                                                                               initBackFaceStencil)));
        }
    } // namespace graphics
} // namespace ouzel
