// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "BlendState.hpp"
#include "Renderer.hpp"

namespace ouzel
{
    namespace graphics
    {
        BlendState::BlendState(Renderer& initRenderer):
            resource(*initRenderer.getDevice())
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
                               uint8_t initColorMask):
            resource(*initRenderer.getDevice()),
            colorBlendSource(initColorBlendSource),
            colorBlendDest(initColorBlendDest),
            colorOperation(initColorOperation),
            alphaBlendSource(initAlphaBlendSource),
            alphaBlendDest(initAlphaBlendDest),
            alphaOperation(initAlphaOperation),
            colorMask(initColorMask),
            enableBlending(initEnableBlending)
        {
            initRenderer.addCommand(std::unique_ptr<Command>(new InitBlendStateCommand(resource,
                                                                                       initEnableBlending,
                                                                                       initColorBlendSource, initColorBlendDest,
                                                                                       initColorOperation,
                                                                                       initAlphaBlendSource, initAlphaBlendDest,
                                                                                       initAlphaOperation,
                                                                                       initColorMask)));
        }
    } // namespace graphics
} // namespace ouzel
