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

            bool getClearColorBuffer() const { return data.clearColorBuffer; }
            virtual void setClearColorBuffer(bool clear);

            bool getClearDepthBuffer() const { return data.clearDepthBuffer; }
            virtual void setClearDepthBuffer(bool clear);

            virtual void setClearColor(Color color);

            uint32_t getSampleCount() const { return data.sampleCount; }

            void setFrameBufferClearedFrame(uint32_t clearedFrame) { frameBufferClearedFrame = clearedFrame; }
            uint32_t getFrameBufferClearedFrame() const { return frameBufferClearedFrame; }

        protected:
            TextureResource();
            virtual bool upload() override;

            bool calculateSizes(const Size2& newSize);
            bool calculateData(const std::vector<uint8_t>& newData, const Size2& newSize);

            struct Level
            {
                Size2 size;
                uint32_t pitch;
                std::vector<uint8_t> data;
            };

            struct Data
            {
                uint32_t dirty = 0;
                Size2 size;
                bool dynamic = false;
                bool mipmaps = false;
                bool mipMapsGenerated = false;
                bool renderTarget = false;
                bool clearColorBuffer = true;
                bool clearDepthBuffer = false;
                std::vector<Level> levels;
                uint32_t sampleCount = 1;
                bool depth = false;
                Color clearColor;
            };

            Data data;

        private:
            Data pendingData;
            std::mutex uploadMutex;

            uint32_t frameBufferClearedFrame = 0;
        };
    } // namespace graphics
} // namespace ouzel
