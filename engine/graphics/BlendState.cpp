// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "BlendState.hpp"
#include "Renderer.hpp"

namespace ouzel::graphics
{
    BlendState::BlendState(Renderer& initRenderer):
        resource(initRenderer.getDevice()->createResource())
    {
    }

    BlendState::BlendState(Renderer& initRenderer,
                           bool initEnableBlending,
                           BlendFactor initColorBlendSource,
                           BlendFactor initColorBlendDest,
                           BlendOperation initColorOperation,
                           BlendFactor initAlphaBlendSource,
                           BlendFactor initAlphaBlendDest,
                           BlendOperation initAlphaOperation,
                           ColorMask initColorMask):
        resource(initRenderer.getDevice()->createResource()),
        colorBlendSource(initColorBlendSource),
        colorBlendDest(initColorBlendDest),
        colorOperation(initColorOperation),
        alphaBlendSource(initAlphaBlendSource),
        alphaBlendDest(initAlphaBlendDest),
        alphaOperation(initAlphaOperation),
        colorMask(initColorMask),
        enableBlending(initEnableBlending)
    {
        initRenderer.addCommand(std::make_unique<InitBlendStateCommand>(resource,
                                                                        initEnableBlending,
                                                                        initColorBlendSource, initColorBlendDest,
                                                                        initColorOperation,
                                                                        initAlphaBlendSource, initAlphaBlendDest,
                                                                        initAlphaOperation,
                                                                        initColorMask));
    }
}
