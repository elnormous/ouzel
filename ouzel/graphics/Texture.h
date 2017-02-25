// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "utils/Noncopyable.h"
#include "math/Color.h"
#include "math/Size2.h"

namespace ouzel
{
    namespace graphics
    {
        class TextureResource;

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

            enum class WrapMode
            {
                CLAMP,
                REPEAT
            };

            static const uint32_t LAYERS = 4;

            Texture();
            virtual ~Texture();

            virtual bool init(const Size2& newSize,
                              bool newDynamic,
                              bool newMipmaps = true,
                              bool newRenderTarget = false,
                              uint32_t newSampleCount = 1,
                              bool newDepth = false);
            virtual bool initFromFile(const std::string& newFilename, bool newDynamic, bool newMipmaps = true);
            virtual bool initFromBuffer(const std::vector<uint8_t>& newData, const Size2& newSize, bool newDynamic, bool newMipmaps = true);

            TextureResource* getResource() const { return resource; }

            const std::string& getFilename() const { return filename; }

            bool setSize(const Size2& newSize);
            bool setData(const std::vector<uint8_t>& newData, const Size2& newSize);

            const Size2& getSize() const { return size; }

            bool isDynamic() const { return dynamic; }

            Filter getFilter() const { return filter; }
            bool setFilter(Filter newFilter);

            WrapMode getWrapMode() const { return wrapMode; }
            bool setWrapMode(WrapMode newWrapMode);

            uint32_t getMaxAnisotropy() const { return maxAnisotropy; }
            bool setMaxAnisotropy(uint32_t newMaxAnisotropy);

            uint32_t getSampleCount() const { return sampleCount; }
            uint32_t getDepth() const { return depth; }

            bool getClearColorBuffer() const { return clearColorBuffer; }
            void setClearColorBuffer(bool clear);

            bool getClearDepthBuffer() const { return clearDepthBuffer; }
            void setClearDepthBuffer(bool clear);

            void setClearColor(Color color);
            Color getClearColor() const { return clearColor; }

        private:
            TextureResource* resource = nullptr;

            std::string filename;

            Size2 size;
            bool dynamic = false;
            bool mipmaps = false;
            bool renderTarget = false;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            bool depth = false;
            uint32_t sampleCount = 1;
            Color clearColor;
            Filter filter = Filter::DEFAULT;
            WrapMode wrapMode = WrapMode::CLAMP;
            uint32_t maxAnisotropy = 0;
        };
    } // namespace graphics
} // namespace ouzel
