// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "graphics/ResourceInterface.h"
#include "graphics/Texture.h"
#include "math/Color.h"
#include "math/Size2.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class TextureInterface: public ResourceInterface
        {
            friend Renderer;
        public:
            virtual ~TextureInterface();

            virtual bool init(const Size2& newSize,
                              bool newDynamic,
                              bool newMipmaps = true,
                              bool newRenderTarget = false,
                              uint32_t newSampleCount = 1,
                              bool newDepth = false,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            virtual bool init(const std::vector<uint8_t>& newData,
                              const Size2& newSize,
                              bool newDynamic,
                              bool newMipmaps = true,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);

            virtual bool setSize(const Size2& newSize);
            const Size2& getSize() const { return size; }

            virtual bool setData(const std::vector<uint8_t>& newData, const Size2& newSize);

            bool isDynamic() const { return dynamic; }

            Texture::Filter getFilter() const { return filter; }
            virtual bool setFilter(Texture::Filter newFilter);

            Texture::Address getAddressX() const { return addressX; }
            virtual bool setAddressX(Texture::Address newAddressX);

            Texture::Address getAddressY() const { return addressY; }
            virtual bool setAddressY(Texture::Address newAddressY);

            uint32_t getMaxAnisotropy() const { return maxAnisotropy; }
            virtual bool setMaxAnisotropy(uint32_t newMaxAnisotropy);

            bool getClearColorBuffer() const { return clearColorBuffer; }
            virtual bool setClearColorBuffer(bool clear);

            bool getClearDepthBuffer() const { return clearDepthBuffer; }
            virtual bool setClearDepthBuffer(bool clear);

            Color getClearColor() const { return clearColor; }
            virtual bool setClearColor(Color color);

            float getClearDepth() const { return clearDepth; }
            virtual bool setClearDepth(float clear);

            uint32_t getSampleCount() const { return sampleCount; }
            bool getDepth() const { return depth; }

            PixelFormat getPixelFormat() const { return pixelFormat; }

            uint32_t getFrameBufferClearedFrame() const { return frameBufferClearedFrame; }
            void setFrameBufferClearedFrame(uint32_t clearedFrame) { frameBufferClearedFrame = clearedFrame; }

        protected:
            TextureInterface();

            bool calculateSizes(const Size2& newSize);
            bool calculateData(const std::vector<uint8_t>& newData);

            struct Level
            {
                Size2 size;
                uint32_t pitch;
                std::vector<uint8_t> data;
            };

            Size2 size;
            bool dynamic = false;
            bool mipmaps = false;
            bool renderTarget = false;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            bool depth = false;
            float clearDepth = 1.0f;
            std::vector<Level> levels;
            uint32_t sampleCount = 1;
            PixelFormat pixelFormat = PixelFormat::RGBA8_UNORM;
            Color clearColor;
            Texture::Filter filter = Texture::Filter::DEFAULT;
            Texture::Address addressX = Texture::Address::CLAMP;
            Texture::Address addressY = Texture::Address::CLAMP;
            uint32_t maxAnisotropy = 0;

            uint32_t frameBufferClearedFrame = 0;
        };
    } // namespace graphics
} // namespace ouzel
