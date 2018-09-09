// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>
#include "graphics/RenderResource.hpp"
#include "graphics/Texture.hpp"
#include "math/Color.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDevice;

        class TextureResource: public RenderResource
        {
        public:
            virtual ~TextureResource();

            inline const Size2& getSize() const { return size; }

            inline uint32_t getFlags() const { return flags; }
            inline uint32_t getMipmaps() const { return mipmaps; }

            inline Texture::Filter getFilter() const { return filter; }
            inline Texture::Address getAddressX() const { return addressX; }
            inline Texture::Address getAddressY() const { return addressY; }
            inline uint32_t getMaxAnisotropy() const { return maxAnisotropy; }
            inline bool getClearColorBuffer() const { return clearColorBuffer; }
            inline bool getClearDepthBuffer() const { return clearDepthBuffer; }
            inline Color getClearColor() const { return clearColor; }
            inline float getClearDepth() const { return clearDepth; }
            inline uint32_t getSampleCount() const { return sampleCount; }
            inline PixelFormat getPixelFormat() const { return pixelFormat; }

        protected:
            TextureResource(RenderDevice& initRenderDevice);

            RenderDevice& renderDevice;
            Size2 size;
            uint32_t flags = 0;
            uint32_t mipmaps = 0;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            Color clearColor;
            float clearDepth = 1.0F;
            std::vector<Texture::Level> levels;
            uint32_t sampleCount = 1;
            PixelFormat pixelFormat = PixelFormat::RGBA8_UNORM;
            Texture::Filter filter = Texture::Filter::DEFAULT;
            Texture::Address addressX = Texture::Address::CLAMP;
            Texture::Address addressY = Texture::Address::CLAMP;
            uint32_t maxAnisotropy = 0;
        };
    } // namespace graphics
} // namespace ouzel
