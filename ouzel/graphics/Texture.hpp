// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_TEXTURE_HPP
#define OUZEL_GRAPHICS_TEXTURE_HPP

#include <vector>
#include "graphics/GraphicsResource.hpp"
#include "graphics/PixelFormat.hpp"
#include "math/Color.hpp"
#include "math/Size.hpp"
#include "utils/Inline.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class Texture final
        {
        public:
            enum class Dimensions
            {
                ONE,
                TWO,
                THREE,
                CUBE
            };

            enum class CubeFace
            {
                POSITIVE_X,
                NEGATIVE_X,
                POSITIVE_Y,
                NEGATIVE_Y,
                POSITIVE_Z,
                NEGATIVE_Z
            };

            enum Flags
            {
                DYNAMIC = 0x01,
                BIND_RENDER_TARGET = 0x02,
                BIND_SHADER = 0x04,
                BIND_SHADER_MSAA = 0x08
            };

            enum class Filter
            {
                DEFAULT,
                POINT,
                LINEAR,
                BILINEAR,
                TRILINEAR
            };

            enum class Address
            {
                CLAMP,
                REPEAT,
                MIRROR_REPEAT
            };

            struct Level final
            {
                Size2U size;
                uint32_t pitch;
                std::vector<uint8_t> data;
            };

            Texture()
            {
            }

            explicit Texture(Renderer& initRenderer);
            Texture(Renderer& initRenderer,
                    const Size2U& newSize,
                    uint32_t newFlags = 0,
                    uint32_t newMipmaps = 0,
                    uint32_t newSampleCount = 1,
                    PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            Texture(Renderer& initRenderer,
                    const std::vector<uint8_t>& newData,
                    const Size2U& newSize,
                    uint32_t newFlags = 0,
                    uint32_t newMipmaps = 0,
                    PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            Texture(Renderer& initRenderer,
                    const std::vector<Level>& newLevels,
                    const Size2U& newSize,
                    uint32_t newFlags = 0,
                    PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);

            FORCEINLINE uintptr_t getResource() const { return resource.getId(); }

            FORCEINLINE const Size2U& getSize() const { return size; }

            void setData(const std::vector<uint8_t>& newData);
            void setData(const std::vector<uint8_t>& newData, CubeFace face);

            FORCEINLINE uint32_t getFlags() const { return flags; }
            FORCEINLINE uint32_t getMipmaps() const { return mipmaps; }

            FORCEINLINE Filter getFilter() const { return filter; }
            void setFilter(Filter newFilter);

            FORCEINLINE Address getAddressX() const { return addressX; }
            void setAddressX(Address newAddressX);

            FORCEINLINE Address getAddressY() const { return addressY; }
            void setAddressY(Address newAddressY);

            FORCEINLINE Address getAddressZ() const { return addressZ; }
            void setAddressZ(Address newAddressZ);

            FORCEINLINE uint32_t getMaxAnisotropy() const { return maxAnisotropy; }
            void setMaxAnisotropy(uint32_t newMaxAnisotropy);

            FORCEINLINE uint32_t getSampleCount() const { return sampleCount; }

            FORCEINLINE PixelFormat getPixelFormat() const { return pixelFormat; }

        private:
            Resource resource;

            Dimensions dimensions = Dimensions::TWO;
            Size2U size;
            uint32_t flags = 0;
            uint32_t mipmaps = 0;
            uint32_t sampleCount = 1;
            PixelFormat pixelFormat = PixelFormat::RGBA8_UNORM;
            Filter filter = Texture::Filter::DEFAULT;
            Address addressX = Texture::Address::CLAMP;
            Address addressY = Texture::Address::CLAMP;
            Address addressZ = Texture::Address::CLAMP;
            uint32_t maxAnisotropy = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_TEXTURE_HPP
