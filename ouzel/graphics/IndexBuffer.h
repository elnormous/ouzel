// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class IndexBuffer: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~IndexBuffer();
            virtual void free() override;

            virtual bool init(bool newDynamic = true);
            virtual bool initFromBuffer(const void* newIndices, uint32_t newIndexSize,
                                        uint32_t newIndexCount, bool newDynamic);

            uint32_t getIndexCount() const { return indexCount; }
            virtual bool setIndexSize(uint32_t newIndexSize);
            uint32_t getIndexSize() const { return indexSize; }

            virtual bool setData(const void* newIndices, uint32_t newIndexCount);

        protected:
            IndexBuffer();
            virtual void update() override;

            enum Dirty
            {
                INDEX_BUFFER_DIRTY = 0x01,
                INDEX_SIZE_DIRTY = 0x02
            };

            struct Data
            {
                uint32_t indexSize = 0;
                std::vector<uint8_t> data;
                bool dynamic = true;
                uint8_t dirty = 0;
            };

            Data uploadData;

        private:
            uint32_t indexCount = 0;
            uint32_t indexSize = 0;

            std::vector<uint8_t> data;

            bool dynamic = true;

            uint8_t dirty = 0;
        };
    } // namespace graphics
} // namespace ouzel
