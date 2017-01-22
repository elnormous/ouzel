// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
#include "graphics/Buffer.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class BufferResource: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~BufferResource();
            virtual void free() override;

            virtual bool init(Buffer::Usage newUsage, bool newDynamic = true);
            virtual bool initFromBuffer(Buffer::Usage newUsage, const void* newIndices, uint32_t newSize, bool newDynamic);

            Buffer::Usage getUsage() const { return usage; }
            uint32_t getSize() const { return size; }

            virtual bool setData(const void* newIndices, uint32_t newSize);

        protected:
            BufferResource();
            virtual void update() override;
            virtual bool upload() override;

            struct Data
            {
                std::vector<uint8_t> data;
                Buffer::Usage usage;
                bool dynamic = true;
            };

            Data uploadData;
            std::mutex uploadMutex;

        private:
            uint32_t size = 0;
            std::vector<uint8_t> data;

            Buffer::Usage usage;
            bool dynamic = true;

            Data currentData;
        };
    } // namespace graphics
} // namespace ouzel
