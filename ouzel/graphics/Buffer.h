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

            bool init(Usage newUsage, bool newDynamic = true);
            bool init(Usage newUsage, const void* newData, uint32_t newSize, bool newDynamic);

            BufferResource* getResource() const { return resource; }

            bool setData(const void* newData, uint32_t newSize);

            bool isDynamic() const;
            Usage getUsage() const;
            uint32_t getSize() const;

        private:
            BufferResource* resource = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
