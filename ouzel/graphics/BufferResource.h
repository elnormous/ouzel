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

            Buffer::Usage getUsage() const { return data.usage; }

        protected:
            BufferResource();
            virtual bool upload() override;

            enum Dirty
            {
                ATTRIBUTES = 0x01,
                DATA = 0x02
            };

            struct Data
            {
                uint32_t dirty = 0;
                std::vector<uint8_t> data;
                Buffer::Usage usage;
                bool dynamic = true;
            };

            Data data;
            std::mutex uploadMutex;

        private:
            Data pendingData;
        };
    } // namespace graphics
} // namespace ouzel
