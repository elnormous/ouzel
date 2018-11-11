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
                                             CompareFunction initCompareFunction):
            renderer(initRenderer),
            resource(renderer.getDevice()->getResourceId()),
            depthTest(initDepthTest),
            depthWrite(initDepthWrite),
            compareFunction(initCompareFunction)
        {
            renderer.addCommand(std::unique_ptr<Command>(new InitDepthStencilStateCommand(resource,
                                                                                          initDepthTest,
                                                                                          initDepthWrite,
                                                                                          initCompareFunction)));
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
                                     CompareFunction newCompareFunction)
        {
            depthTest = newDepthTest;
            depthWrite = newDepthWrite;
            compareFunction = newCompareFunction;

            renderer.addCommand(std::unique_ptr<Command>(new InitDepthStencilStateCommand(resource,
                                                                                          newDepthTest,
                                                                                          newDepthWrite,
                                                                                          newCompareFunction)));
        }
    } // namespace graphics
} // namespace ouzel
