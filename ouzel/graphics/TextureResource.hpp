// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "graphics/RenderResource.hpp"
#include "graphics/Texture.hpp"
#include "math/Color.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class TextureResource: public RenderResource
        {
            friend Renderer;
        public:
            virtual ~TextureResource();

            virtual void init(const Size2& newSize,
                              uint32_t newFlags = 0,
                              uint32_t newMipmaps = 0,
                              uint32_t newSampleCount = 1,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            virtual void init(const std::vector<uint8_t>& newData,
                              const Size2& newSize,
                              uint32_t newFlags = 0,
                              uint32_t newMipmaps = 0,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            virtual void init(const std::vector<Texture::Level>& newLevels,
                              const Size2& newSize,
                              uint32_t newFlags = 0,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);

            virtual void setSize(const Size2& newSize);
            inline const Size2& getSize() const { return size; }

            virtual void setData(const std::vector<uint8_t>& newData, const Size2& newSize);

            inline uint32_t getFlags() const { return flags; }
            inline uint32_t getMipmaps() const { return mipmaps; }

            inline Texture::Filter getFilter() const { return filter; }
            virtual void setFilter(Texture::Filter newFilter);

            inline Texture::Address getAddressX() const { return addressX; }
            virtual void setAddressX(Texture::Address newAddressX);

            inline Texture::Address getAddressY() const { return addressY; }
            virtual void setAddressY(Texture::Address newAddressY);

            inline uint32_t getMaxAnisotropy() const { return maxAnisotropy; }
            virtual void setMaxAnisotropy(uint32_t newMaxAnisotropy);

            inline bool getClearColorBuffer() const { return clearColorBuffer; }
            virtual void setClearColorBuffer(bool clear);

            inline bool getClearDepthBuffer() const { return clearDepthBuffer; }
            virtual void setClearDepthBuffer(bool clear);

            inline Color getClearColor() const { return clearColor; }
            virtual void setClearColor(Color color);

            inline float getClearDepth() const { return clearDepth; }
            virtual void setClearDepth(float clear);

            inline uint32_t getSampleCount() const { return sampleCount; }

            inline PixelFormat getPixelFormat() const { return pixelFormat; }

        protected:
            TextureResource();

            void calculateSizes(const Size2& newSize);
            void calculateData(const std::vector<uint8_t>& newData);

            Size2 size;
            uint32_t flags = 0;
            uint32_t mipmaps = 0;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            float clearDepth = 1.0F;
            std::vector<Texture::Level> levels;
            uint32_t sampleCount = 1;
            PixelFormat pixelFormat = PixelFormat::RGBA8_UNORM;
            Color clearColor;
            Texture::Filter filter = Texture::Filter::DEFAULT;
            Texture::Address addressX = Texture::Address::CLAMP;
            Texture::Address addressY = Texture::Address::CLAMP;
            uint32_t maxAnisotropy = 0;
        };
    } // namespace graphics
} // namespace ouzel
