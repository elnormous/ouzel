// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <vector>
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

            bool init(Usage newUsage, bool newDynamic = true);
            bool init(Usage newUsage, const void* newData, uint32_t newSize, bool newDynamic);
            bool init(Usage newUsage, const std::vector<uint8_t>& newData, bool newDynamic);

            bool setData(const void* newData, uint32_t newSize);
            bool setData(const std::vector<uint8_t>& newData);

            BufferResource* getResource() const { return resource; }

            bool isDynamic() const;
            Usage getUsage() const;

        private:
            Buffer::Usage usage;
            bool dynamic = true;

            BufferResource* resource = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
