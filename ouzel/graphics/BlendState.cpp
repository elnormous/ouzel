// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "BlendState.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        BlendState::BlendState(Renderer& initRenderer):
            renderer(initRenderer)
        {
            resource = renderer.getDevice()->createBlendState();
        }

        BlendState::~BlendState()
        {
            if (resource) renderer.getDevice()->deleteResource(resource);
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
