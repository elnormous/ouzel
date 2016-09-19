// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
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

            virtual bool init(const Size2& newSize, bool newDynamic, bool newMipmaps = true, bool newRenderTarget = false);
            virtual bool initFromFile(const std::string& newFilename, bool newDynamic, bool newMipmaps = true);
            virtual bool initFromBuffer(const std::vector<uint8_t>& newData, const Size2& newSize, bool newDynamic, bool newMipmaps = true);

            const std::string& getFilename() const { return filename; }

            virtual bool setData(const std::vector<uint8_t>& newData, const Size2& newSize);

            const Size2& getSize() const { return size; }

            bool isDynamic() const { return dynamic; }
            bool isFlipped() const { return flipped; }
            void setFlipped(bool newFlipped) { flipped = newFlipped; }

        protected:
            Texture();
            virtual void update() override;

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
                bool dirty = false;
                std::vector<Level> levels;
            };

            Data uploadData;

        private:
            std::string filename;
            std::vector<Level> levels;

            Size2 size;
            bool dynamic = false;
            bool mipmaps = false;
            bool renderTarget = false;
            bool flipped = false;
            bool dirty = false;
            bool mipMapsGenerated = false;

        };
    } // namespace graphics
} // namespace ouzel
