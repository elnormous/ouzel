// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_TEXTURE_HPP
#define OUZEL_GRAPHICS_TEXTURE_HPP

#include <vector>
#include <utility>
#include "graphics/RenderDevice.hpp"
#include "graphics/CubeFace.hpp"
#include "graphics/Flags.hpp"
#include "graphics/PixelFormat.hpp"
#include "graphics/SamplerAddressMode.hpp"
#include "graphics/SamplerFilter.hpp"
#include "graphics/TextureType.hpp"
#include "math/Color.hpp"
#include "math/Size.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class Texture final
        {
        public:
            Texture() = default;

            explicit Texture(Renderer& initRenderer);
            Texture(Renderer& initRenderer,
                    const Size2U& initSize,
                    uint32_t initFlags = 0,
                    uint32_t initMipmaps = 0,
                    uint32_t initSampleCount = 1,
                    PixelFormat initPixelFormat = PixelFormat::RGBA8UNorm);
            Texture(Renderer& initRenderer,
                    const std::vector<uint8_t>& initData,
                    const Size2U& initSize,
                    uint32_t initFlags = 0,
                    uint32_t initMipmaps = 0,
                    PixelFormat initPixelFormat = PixelFormat::RGBA8UNorm);
            Texture(Renderer& initRenderer,
                    const std::vector<std::pair<Size2U, std::vector<uint8_t>>>& initLevels,
                    const Size2U& initSize,
                    uint32_t initFlags = 0,
                    PixelFormat initPixelFormat = PixelFormat::RGBA8UNorm);

            inline auto& getResource() const noexcept { return resource; }

            inline auto& getSize() const noexcept { return size; }

            void setData(const std::vector<uint8_t>& newData, CubeFace face = CubeFace::PositiveX);

            inline auto getFlags() const noexcept { return flags; }
            inline auto getMipmaps() const noexcept { return mipmaps; }

            inline auto getFilter() const noexcept { return filter; }
            void setFilter(SamplerFilter newFilter);

            inline auto getAddressX() const noexcept { return addressX; }
            void setAddressX(SamplerAddressMode newAddressX);

            inline auto getAddressY() const noexcept { return addressY; }
            void setAddressY(SamplerAddressMode newAddressY);

            inline auto getAddressZ() const noexcept { return addressZ; }
            void setAddressZ(SamplerAddressMode newAddressZ);

            inline auto getBorderColor() const noexcept { return borderColor; }
            void setBorderColor(Color newBorderColor);

            inline auto getMaxAnisotropy() const noexcept { return maxAnisotropy; }
            void setMaxAnisotropy(uint32_t newMaxAnisotropy);

            inline auto getSampleCount() const noexcept { return sampleCount; }

            inline auto getPixelFormat() const noexcept { return pixelFormat; }

        private:
            Renderer* renderer = nullptr;
            RenderDevice::Resource resource;

            TextureType type = TextureType::TwoDimensional;
            Size2U size;
            uint32_t flags = 0;
            uint32_t mipmaps = 0;
            uint32_t sampleCount = 1;
            PixelFormat pixelFormat = PixelFormat::RGBA8UNorm;
            SamplerFilter filter = SamplerFilter::Default;
            SamplerAddressMode addressX = SamplerAddressMode::ClampToEdge;
            SamplerAddressMode addressY = SamplerAddressMode::ClampToEdge;
            SamplerAddressMode addressZ = SamplerAddressMode::ClampToEdge;
            Color borderColor;
            uint32_t maxAnisotropy = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_TEXTURE_HPP
