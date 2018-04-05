// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

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

            bool init(Usage newUsage, uint32_t newFlags = 0, uint32_t newSize = 0);
            bool init(Usage newUsage, const void* newData, uint32_t newSize, uint32_t newFlags = 0);
            bool init(Usage newUsage, const std::vector<uint8_t>& newData, uint32_t newFlags = 0);

            bool setData(const void* newData, uint32_t newSize);
            bool setData(const std::vector<uint8_t>& newData);

            inline BufferResource* getResource() const { return resource; }

            inline uint32_t getFlags() const { return flags; }
            inline Usage getUsage() const { return usage; }

        private:
            BufferResource* resource = nullptr;

            Buffer::Usage usage;
            uint32_t flags = 0;
        };
    } // namespace graphics
} // namespace ouzel
