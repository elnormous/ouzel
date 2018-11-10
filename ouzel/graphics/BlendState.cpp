// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "BlendState.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        BlendState::BlendState(Renderer& initRenderer):
            renderer(initRenderer),
            resource(renderer.getDevice()->getResourceId())
        {
        }

        BlendState::BlendState(Renderer& initRenderer,
                               bool initEnableBlending,
                               Factor initColorBlendSource, Factor initColorBlendDest,
                               Operation initColorOperation,
                               Factor initAlphaBlendSource, Factor initAlphaBlendDest,
                               Operation initAlphaOperation,
                               uint8_t initColorMask):
            renderer(initRenderer),
            resource(renderer.getDevice()->getResourceId()),
            colorBlendSource(initColorBlendSource),
            colorBlendDest(initColorBlendDest),
            colorOperation(initColorOperation),
            alphaBlendSource(initAlphaBlendSource),
            alphaBlendDest(initAlphaBlendDest),
            alphaOperation(initAlphaOperation),
            colorMask(initColorMask),
            enableBlending(initEnableBlending)
        {
            RenderDevice* renderDevice = renderer.getDevice();

            renderDevice->addCommand(std::unique_ptr<Command>(new InitBlendStateCommand(resource,
                                                                                        initEnableBlending,
                                                                                        initColorBlendSource, initColorBlendDest,
                                                                                        initColorOperation,
                                                                                        initAlphaBlendSource, initAlphaBlendDest,
                                                                                        initAlphaOperation,
                                                                                        initColorMask)));
        }

        BlendState::~BlendState()
        {
            if (resource)
            {
                RenderDevice* renderDevice = renderer.getDevice();
                renderDevice->addCommand(std::unique_ptr<Command>(new DeleteResourceCommand(resource)));
                renderDevice->deleteResourceId(resource);
            }
        }

        void BlendState::init(bool newEnableBlending,
                              Factor newColorBlendSource, Factor newColorBlendDest,
                              Operation newColorOperation,
                              Factor newAlphaBlendSource, Factor newAlphaBlendDest,
                              Operation newAlphaOperation,
                              uint8_t newColorMask)
        {
            enableBlending = newEnableBlending;
            colorBlendSource = newColorBlendSource;
            colorBlendDest = newColorBlendDest;
            colorOperation = newColorOperation;
            alphaBlendSource = newAlphaBlendSource;
            alphaBlendDest = newAlphaBlendDest;
            alphaOperation = newAlphaOperation;
            colorMask = newColorMask;

            RenderDevice* renderDevice = renderer.getDevice();

            renderDevice->addCommand(std::unique_ptr<Command>(new InitBlendStateCommand(resource,
                                                                                        newEnableBlending,
                                                                                        newColorBlendSource, newColorBlendDest,
                                                                                        newColorOperation,
                                                                                        newAlphaBlendSource, newAlphaBlendDest,
                                                                                        newAlphaOperation,
                                                                                        newColorMask)));
        }
    } // namespace graphics
} // namespace ouzel
