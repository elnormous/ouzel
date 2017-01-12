// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
#include "math/Color.h"
#include "math/Size2.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class Texture: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            static const uint32_t LAYERS = 4;

            virtual ~Texture();
            virtual void free() override;

            virtual bool init(const Size2& newSize, bool newDynamic, bool newMipmaps = true, bool newRenderTarget = false, uint32_t newSampleCount = 1, bool newDepth = false);
            virtual bool initFromFile(const std::string& newFilename, bool newDynamic, bool newMipmaps = true);
            virtual bool initFromBuffer(const std::vector<uint8_t>& newData, const Size2& newSize, bool newDynamic, bool newMipmaps = true);

            const std::string& getFilename() const { return filename; }

            virtual bool setSize(const Size2& newSize);
            virtual bool setData(const std::vector<uint8_t>& newData, const Size2& newSize);

            const Size2& getSize() const { return size; }

            bool isDynamic() const { return dynamic; }

            uint32_t getSampleCount() const { return sampleCount; }
            uint32_t getDepth() const { return depth; }

            virtual void setClearColorBuffer(bool clear);
            virtual bool getClearColorBuffer() const { return clearColorBuffer; }

            virtual void setClearDepthBuffer(bool clear);
            virtual bool getClearDepthBuffer() const { return clearDepthBuffer; }

            virtual void setClearColor(Color color);
            virtual Color getClearColor() const { return clearColor; }

            void setFrameBufferClearedFrame(uint32_t clearedFrame) { frameBufferClearedFrame = clearedFrame; }
            uint32_t getFrameBufferClearedFrame() const { return frameBufferClearedFrame; }

        protected:
            Texture();
            virtual void update() override;
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
                Size2 size;
                bool dynamic = false;
                bool mipmaps = false;
                bool renderTarget = false;
                bool clearColorBuffer = true;
                bool clearDepthBuffer = false;
                std::vector<Level> levels;
                uint32_t sampleCount = 1;
                bool depth = false;
                Color clearColor;
            };

            Data uploadData;
            std::mutex uploadMutex;

            uint32_t frameBufferClearedFrame = 0;

        private:
            std::string filename;
            std::vector<Level> levels;

            Size2 size;
            bool dynamic = false;
            bool mipmaps = false;
            bool renderTarget = false;
            bool mipMapsGenerated = false;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            uint32_t sampleCount = 1;
            bool depth = false;
            Color clearColor;

            Data currentData;
        };
    } // namespace graphics
} // namespace ouzel
