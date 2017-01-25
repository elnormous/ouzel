// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "utils/Noncopyable.h"

namespace ouzel
{
    namespace graphics
    {
        class BufferResource;

        class Buffer: public Noncopyable
        {
        public:
            enum class Usage
            {
                NONE,
                INDEX,
                VERTEX
            };

            Buffer();
            virtual ~Buffer();

            bool init(Buffer::Usage newUsage, bool newDynamic = true);
            bool initFromBuffer(Buffer::Usage newUsage, const void* newData, uint32_t newSize, bool newDynamic);

            BufferResource* getResource() const { return resource; }

            Buffer::Usage getUsage() const { return usage; }
            uint32_t getSize() const { return size; }

            bool setData(const void* newData, uint32_t newSize);

        private:
            BufferResource* resource = nullptr;

            Buffer::Usage usage;
            bool dynamic = true;
            uint32_t size = 0;
        };
    } // namespace graphics
} // namespace ouzel
