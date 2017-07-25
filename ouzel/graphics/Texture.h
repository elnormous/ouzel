// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "utils/Noncopyable.h"
#include "graphics/PixelFormat.h"
#include "math/Color.h"
#include "math/Size2.h"

namespace ouzel
{
    namespace graphics
    {
        class TextureInterface;

        class Texture: public Noncopyable
        {
        public:
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

            static const uint32_t LAYERS = 4;

            Texture();
            virtual ~Texture();

            virtual bool init(const Size2& newSize,
                              bool newDynamic,
                              bool newMipmaps = true,
                              bool newRenderTarget = false,
                              uint32_t newSampleCount = 1,
                              bool newDepth = false,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            virtual bool init(const std::string& newFilename,
                              bool newDynamic,
                              bool newMipmaps = true,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            virtual bool init(const std::vector<uint8_t>& newData,
                              const Size2& newSize, bool newDynamic,
                              bool newMipmaps = true,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);

            TextureInterface* getResource() const { return resource; }

            const std::string& getFilename() const { return filename; }

            const Size2& getSize() const;
            bool setSize(const Size2& newSize);

            bool setData(const std::vector<uint8_t>& newData, const Size2& newSize);

            bool isDynamic() const;

            Filter getFilter() const;
            bool setFilter(Filter newFilter);

            Address getAddressX() const;
            bool setAddressX(Address newAddressX);

            Address getAddressY() const;
            bool setAddressY(Address newAddressY);

            uint32_t getMaxAnisotropy() const;
            bool setMaxAnisotropy(uint32_t newMaxAnisotropy);

            uint32_t getSampleCount() const;
            bool getDepth() const;
            PixelFormat getPixelFormat() const;

            bool getClearColorBuffer() const;
            void setClearColorBuffer(bool clear);

            bool getClearDepthBuffer() const;
            void setClearDepthBuffer(bool clear);

            Color getClearColor() const;
            void setClearColor(Color color);

        private:
            Size2 size;
            bool dynamic = false;
            bool mipmaps = false;
            bool renderTarget = false;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            bool depth = false;
            uint32_t sampleCount = 1;
            PixelFormat pixelFormat = PixelFormat::RGBA8_UNORM;
            Color clearColor;
            Filter filter = Texture::Filter::DEFAULT;
            Address addressX = Texture::Address::CLAMP;
            Address addressY = Texture::Address::CLAMP;
            uint32_t maxAnisotropy = 0;

            std::string filename;

            TextureInterface* resource = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
