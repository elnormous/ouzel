// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
#include "graphics/Texture.h"
#include "math/Color.h"
#include "math/Size2.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class TextureResource: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~TextureResource();

            virtual bool init(const Size2& newSize, bool newDynamic, bool newMipmaps = true, bool newRenderTarget = false, uint32_t newSampleCount = 1, bool newDepth = false);
            virtual bool initFromBuffer(const std::vector<uint8_t>& newData, const Size2& newSize, bool newDynamic, bool newMipmaps = true);

            virtual bool setSize(const Size2& newSize);
            virtual bool setData(const std::vector<uint8_t>& newData, const Size2& newSize);

            Texture::Filter getFilter() const { return filter; }
            bool setFilter(Texture::Filter newFilter);

            Texture::Address getAddressX() const { return addressX; }
            bool setAddressX(Texture::Address newAddressX);

            Texture::Address getAddressY() const { return addressY; }
            bool setAddressY(Texture::Address newAddressY);

            uint32_t getMaxAnisotropy() const { return maxAnisotropy; }
            bool setMaxAnisotropy(uint32_t newMaxAnisotropy);

            bool getClearColorBuffer() const { return clearColorBuffer; }
            virtual void setClearColorBuffer(bool clear);

            bool getClearDepthBuffer() const { return clearDepthBuffer; }
            virtual void setClearDepthBuffer(bool clear);

            virtual void setClearColor(Color color);

            uint32_t getSampleCount() const { return sampleCount; }

            void setFrameBufferClearedFrame(uint32_t clearedFrame) { frameBufferClearedFrame = clearedFrame; }
            uint32_t getFrameBufferClearedFrame() const { return frameBufferClearedFrame; }

        protected:
            TextureResource();

            bool calculateSizes(const Size2& newSize);
            bool calculateData(const std::vector<uint8_t>& newData, const Size2& newSize);

            struct Level
            {
                Size2 size;
                uint32_t pitch;
                std::vector<uint8_t> data;
            };

            static const uint32_t DIRTY_DATA = 1;
            static const uint32_t DIRTY_PARAMETERS = 2;

            uint32_t dirty = 0;

            Size2 size;
            bool dynamic = false;
            bool mipmaps = false;
            bool renderTarget = false;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            bool depth = false;
            std::vector<Level> levels;
            uint32_t sampleCount = 1;
            Color clearColor;
            Texture::Filter filter = Texture::Filter::DEFAULT;
            Texture::Address addressX = Texture::Address::CLAMP;
            Texture::Address addressY = Texture::Address::CLAMP;
            uint32_t maxAnisotropy = 0;

            std::mutex uploadMutex;

            uint32_t frameBufferClearedFrame = 0;
        };
    } // namespace graphics
} // namespace ouzel
