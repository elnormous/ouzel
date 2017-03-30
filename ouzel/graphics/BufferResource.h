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

            bool init(Buffer::Usage newUsage, bool newDynamic = true);
            bool initFromBuffer(Buffer::Usage newUsage, const void* newData, uint32_t newSize, bool newDynamic);

            bool setData(const void* newData, uint32_t newSize);

            bool isDynamic() const { return dynamic; }
            Buffer::Usage getUsage() const { return usage; }
            uint32_t getSize() const { return static_cast<uint32_t>(data.size()); }

        protected:
            BufferResource();

            enum Dirty
            {
                ATTRIBUTES = 0x01,
                DATA = 0x02
            };

            uint32_t dirty = 0;
            std::vector<uint8_t> data;
            Buffer::Usage usage;
            bool dynamic = true;

            std::mutex uploadMutex;
        };
    } // namespace graphics
} // namespace ouzel
