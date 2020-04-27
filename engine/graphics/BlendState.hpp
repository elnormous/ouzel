// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_BLENDSTATE_HPP
#define OUZEL_GRAPHICS_BLENDSTATE_HPP

#include "RenderDevice.hpp"
#include "BlendFactor.hpp"
#include "BlendOperation.hpp"
#include "ColorMask.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class BlendState final
        {
        public:
            BlendState() = default;

            explicit BlendState(Renderer& initRenderer);
            BlendState(Renderer& initRenderer,
                       bool initEnableBlending,
                       BlendFactor initColorBlendSource, BlendFactor initColorBlendDest,
                       BlendOperation initColorOperation,
                       BlendFactor initAlphaBlendSource, BlendFactor initAlphaBlendDest,
                       BlendOperation initAlphaOperation,
                       ColorMask initColorMask = ColorMask::All);

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

            BlendFactor colorBlendSource = BlendFactor::One;
            BlendFactor colorBlendDest = BlendFactor::Zero;
            BlendOperation colorOperation = BlendOperation::Add;
            BlendFactor alphaBlendSource = BlendFactor::One;
            BlendFactor alphaBlendDest = BlendFactor::Zero;
            BlendOperation alphaOperation = BlendOperation::Add;
            ColorMask colorMask = ColorMask::All;
            bool enableBlending = false;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_BLENDSTATE_HPP
