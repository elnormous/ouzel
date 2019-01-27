// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "DepthStencilState.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        DepthStencilState::DepthStencilState(Renderer& initRenderer):
            renderer(initRenderer),
            resource(renderer.getDevice()->getResourceId())
        {
        }

        DepthStencilState::DepthStencilState(Renderer& initRenderer,
                                             bool initDepthTest,
                                             bool initDepthWrite,
                                             CompareFunction initCompareFunction,
                                             uint32_t initStencilReadMask,
                                             uint32_t initStencilWriteMask):
            renderer(initRenderer),
            resource(renderer.getDevice()->getResourceId()),
            depthTest(initDepthTest),
            depthWrite(initDepthWrite),
            compareFunction(initCompareFunction),
            stencilReadMask(initStencilReadMask),
            stencilWriteMask(initStencilWriteMask)
        {
            renderer.addCommand(std::unique_ptr<Command>(new InitDepthStencilStateCommand(resource,
                                                                                          initDepthTest,
                                                                                          initDepthWrite,
                                                                                          initCompareFunction,
                                                                                          initStencilReadMask,
                                                                                          initStencilWriteMask)));
        }

        DepthStencilState::~DepthStencilState()
        {
            if (resource)
            {
                renderer.addCommand(std::unique_ptr<Command>(new DeleteResourceCommand(resource)));
                RenderDevice* renderDevice = renderer.getDevice();
                renderDevice->deleteResourceId(resource);
            }
        }

        void DepthStencilState::init(bool newDepthTest,
                                     bool newDepthWrite,
                                     CompareFunction newCompareFunction,
                                     uint32_t newStencilReadMask,
                                     uint32_t newStencilWriteMask)
        {
            depthTest = newDepthTest;
            depthWrite = newDepthWrite;
            compareFunction = newCompareFunction;
            stencilReadMask = newStencilReadMask;
            stencilWriteMask = newStencilWriteMask;

            renderer.addCommand(std::unique_ptr<Command>(new InitDepthStencilStateCommand(resource,
                                                                                          newDepthTest,
                                                                                          newDepthWrite,
                                                                                          newCompareFunction,
                                                                                          newStencilReadMask,
                                                                                          newStencilWriteMask)));
        }
    } // namespace graphics
} // namespace ouzel
