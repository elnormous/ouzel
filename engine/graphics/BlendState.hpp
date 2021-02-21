// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_BLENDSTATE_HPP
#define OUZEL_GRAPHICS_BLENDSTATE_HPP

#include "RenderDevice.hpp"
#include "BlendFactor.hpp"
#include "BlendOperation.hpp"
#include "ColorMask.hpp"

namespace ouzel::graphics
{
    class Graphics;

    class BlendState final
    {
    public:
        BlendState() = default;

        explicit BlendState(Graphics& initGraphics);
        BlendState(Graphics& initGraphics,
                   bool initEnableBlending,
                   BlendFactor initColorBlendSource, BlendFactor initColorBlendDest,
                   BlendOperation initColorOperation,
                   BlendFactor initAlphaBlendSource, BlendFactor initAlphaBlendDest,
                   BlendOperation initAlphaOperation,
                   ColorMask initColorMask = ColorMask::all);

        auto& getResource() const noexcept { return resource; }

        auto isBlendingEnabled() const noexcept { return enableBlending; }

        auto getColorBlendSource() const noexcept { return colorBlendSource; }
        auto getColorBlendDest() const noexcept { return colorBlendDest; }
        auto getColorOperation() const noexcept { return colorOperation; }
        auto getAlphaBlendSource() const noexcept { return alphaBlendSource; }
        auto getAlphaBlendDest() const noexcept { return alphaBlendDest; }
        auto getAlphaOperation() const noexcept { return alphaOperation; }

        auto getColorMask() const noexcept { return colorMask; }

    private:
        RenderDevice::Resource resource;

        BlendFactor colorBlendSource = BlendFactor::one;
        BlendFactor colorBlendDest = BlendFactor::zero;
        BlendOperation colorOperation = BlendOperation::add;
        BlendFactor alphaBlendSource = BlendFactor::one;
        BlendFactor alphaBlendDest = BlendFactor::zero;
        BlendOperation alphaOperation = BlendOperation::add;
        ColorMask colorMask = ColorMask::all;
        bool enableBlending = false;
    };
}

#endif // OUZEL_GRAPHICS_BLENDSTATE_HPP
