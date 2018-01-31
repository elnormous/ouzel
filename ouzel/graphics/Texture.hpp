// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "graphics/PixelFormat.hpp"
#include "math/Color.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace graphics
    {
        class TextureResource;

        class Texture
        {
        public:
            enum Flags
            {
                DYNAMIC = 0x01,
                RENDER_TARGET = 0x02,
                DEPTH_BUFFER = 0x04
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

            struct Level
            {
                Size2 size;
                uint32_t pitch;
                std::vector<uint8_t> data;
            };

            static const uint32_t LAYERS = 4;

            Texture();
            virtual ~Texture();

            Texture(const Texture&) = delete;
            Texture& operator=(const Texture&) = delete;

            Texture(const Texture&&) = delete;
            Texture& operator=(const Texture&&) = delete;

            bool init(const Size2& newSize,
                      uint32_t newFlags = 0,
                      uint32_t newMipmaps = 0,
                      uint32_t newSampleCount = 1,
                      PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            bool init(const std::string& filename,
                      uint32_t newFlags = 0,
                      uint32_t newMipmaps = 0,
                      PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            bool init(const std::vector<uint8_t>& newData,
                      const Size2& newSize,
                      uint32_t newFlags = 0,
                      uint32_t newMipmaps = 0,
                      PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            bool init(const std::vector<Level>& newLevels,
                      const Size2& newSize,
                      uint32_t newFlags = 0,
                      PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);

            TextureResource* getResource() const { return resource; }

            const Size2& getSize() const;
            bool setSize(const Size2& newSize);

            bool setData(const std::vector<uint8_t>& newData, const Size2& newSize);

            uint32_t getFlags() const { return flags; }
            uint32_t getMipmaps() const { return mipmaps; }

            Filter getFilter() const;
            bool setFilter(Filter newFilter);

            Address getAddressX() const;
            bool setAddressX(Address newAddressX);

            Address getAddressY() const;
            bool setAddressY(Address newAddressY);

            uint32_t getMaxAnisotropy() const;
            bool setMaxAnisotropy(uint32_t newMaxAnisotropy);

            uint32_t getSampleCount() const;
            PixelFormat getPixelFormat() const;

            bool getClearColorBuffer() const;
            void setClearColorBuffer(bool clear);

            bool getClearDepthBuffer() const;
            void setClearDepthBuffer(bool clear);

            Color getClearColor() const;
            void setClearColor(Color color);

            float getClearDepth() const;
            void setClearDepth(float depth);

        private:
            TextureResource* resource = nullptr;

            Size2 size;
            uint32_t flags = 0;
            uint32_t mipmaps = 0;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            float clearDepth = 1.0f;
            uint32_t sampleCount = 1;
            PixelFormat pixelFormat = PixelFormat::RGBA8_UNORM;
            Color clearColor;
            Filter filter = Texture::Filter::DEFAULT;
            Address addressX = Texture::Address::CLAMP;
            Address addressY = Texture::Address::CLAMP;
            uint32_t maxAnisotropy = 0;
        };
    } // namespace graphics
} // namespace ouzel
