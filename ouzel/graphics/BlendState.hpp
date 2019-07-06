// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_BLENDSTATE_HPP
#define OUZEL_GRAPHICS_BLENDSTATE_HPP

#include "graphics/RenderDevice.hpp"
#include "graphics/BlendFactor.hpp"
#include "graphics/BlendOperation.hpp"
#include "graphics/ColorMask.hpp"

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
                       uint8_t initColorMask = ColorMask::COLOR_MASK_ALL);

            inline const RenderDevice::Resource& getResource() const { return resource; }

            inline auto isBlendingEnabled() const { return enableBlending; }

            inline auto getColorBlendSource() const { return colorBlendSource; }
            inline auto getColorBlendDest() const { return colorBlendDest; }
            inline auto getColorOperation() const { return colorOperation; }
            inline auto getAlphaBlendSource() const { return alphaBlendSource; }
            inline auto getAlphaBlendDest() const { return alphaBlendDest; }
            inline auto getAlphaOperation() const { return alphaOperation; }

            inline auto getColorMask() const { return colorMask; }

        private:
            RenderDevice::Resource resource;

            BlendFactor colorBlendSource = BlendFactor::ONE;
            BlendFactor colorBlendDest = BlendFactor::ZERO;
            BlendOperation colorOperation = BlendOperation::ADD;
            BlendFactor alphaBlendSource = BlendFactor::ONE;
            BlendFactor alphaBlendDest = BlendFactor::ZERO;
            BlendOperation alphaOperation = BlendOperation::ADD;
            uint8_t colorMask = ColorMask::COLOR_MASK_ALL;
            bool enableBlending = false;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_BLENDSTATE_HPP
