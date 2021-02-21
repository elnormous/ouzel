// Ouzel by Elviss Strazdins

#include "BlendState.hpp"
#include "Graphics.hpp"

namespace ouzel::graphics
{
    BlendState::BlendState(Graphics& initGraphics):
        resource(*initGraphics.getDevice())
    {
    }

    BlendState::BlendState(Graphics& initGraphics,
                           bool initEnableBlending,
                           BlendFactor initColorBlendSource,
                           BlendFactor initColorBlendDest,
                           BlendOperation initColorOperation,
                           BlendFactor initAlphaBlendSource,
                           BlendFactor initAlphaBlendDest,
                           BlendOperation initAlphaOperation,
                           ColorMask initColorMask):
        resource(*initGraphics.getDevice()),
        colorBlendSource(initColorBlendSource),
        colorBlendDest(initColorBlendDest),
        colorOperation(initColorOperation),
        alphaBlendSource(initAlphaBlendSource),
        alphaBlendDest(initAlphaBlendDest),
        alphaOperation(initAlphaOperation),
        colorMask(initColorMask),
        enableBlending(initEnableBlending)
    {
        initGraphics.addCommand(std::make_unique<InitBlendStateCommand>(resource,
                                                                        initEnableBlending,
                                                                        initColorBlendSource, initColorBlendDest,
                                                                        initColorOperation,
                                                                        initAlphaBlendSource, initAlphaBlendDest,
                                                                        initAlphaOperation,
                                                                        initColorMask));
    }
}
