// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_TEXTURE_HPP
#define OUZEL_GRAPHICS_TEXTURE_HPP

#include <vector>
#include <utility>
#include "RenderDevice.hpp"
#include "CubeFace.hpp"
#include "Flags.hpp"
#include "PixelFormat.hpp"
#include "SamplerAddressMode.hpp"
#include "SamplerFilter.hpp"
#include "TextureType.hpp"
#include "../math/Color.hpp"
#include "../math/Size.hpp"

namespace ouzel::graphics
{
    class Graphics;

    class Texture final
    {
    public:
        Texture() = default;

        explicit Texture(Graphics& initGraphics);
        Texture(Graphics& initGraphics,
                const Size2U& initSize,
                Flags initFlags = Flags::none,
                std::uint32_t initMipmaps = 0,
                std::uint32_t initSampleCount = 1,
                PixelFormat initPixelFormat = PixelFormat::rgba8UnsignedNorm);
        Texture(Graphics& initGraphics,
                const std::vector<std::uint8_t>& initData,
                const Size2U& initSize,
                Flags initFlags = Flags::none,
                std::uint32_t initMipmaps = 0,
                PixelFormat initPixelFormat = PixelFormat::rgba8UnsignedNorm);
        Texture(Graphics& initGraphics,
                const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& initLevels,
                const Size2U& initSize,
                Flags initFlags = Flags::none,
                PixelFormat initPixelFormat = PixelFormat::rgba8UnsignedNorm);

        auto& getResource() const noexcept { return resource; }

        auto& getSize() const noexcept { return size; }

        void setData(const std::vector<std::uint8_t>& newData, CubeFace face = CubeFace::positiveX);

        auto getFlags() const noexcept { return flags; }
        auto getMipmaps() const noexcept { return mipmaps; }

        auto getFilter() const noexcept { return filter; }
        void setFilter(SamplerFilter newFilter);

        auto getAddressX() const noexcept { return addressX; }
        void setAddressX(SamplerAddressMode newAddressX);

        auto getAddressY() const noexcept { return addressY; }
        void setAddressY(SamplerAddressMode newAddressY);

        auto getAddressZ() const noexcept { return addressZ; }
        void setAddressZ(SamplerAddressMode newAddressZ);

        auto getBorderColor() const noexcept { return borderColor; }
        void setBorderColor(Color newBorderColor);

        auto getMaxAnisotropy() const noexcept { return maxAnisotropy; }
        void setMaxAnisotropy(std::uint32_t newMaxAnisotropy);

        auto getSampleCount() const noexcept { return sampleCount; }

        auto getPixelFormat() const noexcept { return pixelFormat; }

    private:
        Graphics* graphics = nullptr;
        RenderDevice::Resource resource;

        TextureType type = TextureType::twoDimensional;
        Size2U size;
        Flags flags = Flags::none;
        std::uint32_t mipmaps = 0;
        std::uint32_t sampleCount = 1;
        PixelFormat pixelFormat = PixelFormat::rgba8UnsignedNorm;
        SamplerFilter filter = SamplerFilter::point;
        SamplerAddressMode addressX = SamplerAddressMode::clampToEdge;
        SamplerAddressMode addressY = SamplerAddressMode::clampToEdge;
        SamplerAddressMode addressZ = SamplerAddressMode::clampToEdge;
        Color borderColor;
        std::uint32_t maxAnisotropy = 0;
    };
}

#endif // OUZEL_GRAPHICS_TEXTURE_HPP
