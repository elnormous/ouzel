// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "BlendState.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        BlendState::BlendState(Renderer& initRenderer):
            resource(initRenderer)
        {
        }

        BlendState::BlendState(Renderer& initRenderer,
                               bool initEnableBlending,
                               Factor initColorBlendSource, Factor initColorBlendDest,
                               Operation initColorOperation,
                               Factor initAlphaBlendSource, Factor initAlphaBlendDest,
                               Operation initAlphaOperation,
                               uint8_t initColorMask):
            resource(initRenderer),
            colorBlendSource(initColorBlendSource),
            colorBlendDest(initColorBlendDest),
            colorOperation(initColorOperation),
            alphaBlendSource(initAlphaBlendSource),
            alphaBlendDest(initAlphaBlendDest),
            alphaOperation(initAlphaOperation),
            colorMask(initColorMask),
            enableBlending(initEnableBlending)
        {
            initRenderer.addCommand(std::unique_ptr<Command>(new InitBlendStateCommand(resource.getId(),
                                                                                       initEnableBlending,
                                                                                       initColorBlendSource, initColorBlendDest,
                                                                                       initColorOperation,
                                                                                       initAlphaBlendSource, initAlphaBlendDest,
                                                                                       initAlphaOperation,
                                                                                       initColorMask)));
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

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new InitBlendStateCommand(resource.getId(),
                                                                                                      newEnableBlending,
                                                                                                      newColorBlendSource, newColorBlendDest,
                                                                                                      newColorOperation,
                                                                                                      newAlphaBlendSource, newAlphaBlendDest,
                                                                                                      newAlphaOperation,
                                                                                                      newColorMask)));
        }
    } // namespace graphics
} // namespace ouzel
