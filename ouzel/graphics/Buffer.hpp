// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>

namespace ouzel
{
    namespace graphics
    {
        class BufferResource;

        class Buffer final
        {
        public:
            enum Flags
            {
                DYNAMIC = 0x01
            };

            enum class Usage
            {
                NONE,
                INDEX,
                VERTEX
            };

            Buffer();
            ~Buffer();

            Buffer(const Buffer&) = delete;
            Buffer& operator=(const Buffer&) = delete;

            Buffer(Buffer&&) = delete;
            Buffer& operator=(Buffer&&) = delete;

            void init(Usage newUsage, uint32_t newFlags, uint32_t newSize = 0);
            void init(Usage newUsage, uint32_t newFlags, const void* newData, uint32_t newSize);
            void init(Usage newUsage, uint32_t newFlags, const std::vector<uint8_t>& newData, uint32_t newSize);

            void setData(const void* newData, uint32_t newSize);
            void setData(const std::vector<uint8_t>& newData);

            inline BufferResource* getResource() const { return resource; }

            inline Usage getUsage() const { return usage; }
            inline uint32_t getFlags() const { return flags; }
            inline uint32_t getSize() const { return size; }

        private:
            BufferResource* resource = nullptr;

            Buffer::Usage usage;
            uint32_t flags = 0;
            uint32_t size = 0;
        };
    } // namespace graphics
} // namespace ouzel
