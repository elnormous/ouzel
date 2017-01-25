// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        BufferResource::BufferResource()
        {
        }

        BufferResource::~BufferResource()
        {
        }

        bool BufferResource::init(Buffer::Usage newUsage, bool newDynamic)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            pendingData.usage = newUsage;
            pendingData.dynamic = newDynamic;
            
            pendingData.dirty |= ATTRIBUTES;

            return true;
        }

        bool BufferResource::initFromBuffer(Buffer::Usage newUsage, const void* newData, uint32_t newSize, bool newDynamic)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);
            
            pendingData.usage = newUsage;
            pendingData.dynamic = newDynamic;

            if (newData && newSize)
            {
                pendingData.data.assign(static_cast<const uint8_t*>(newData),
                                        static_cast<const uint8_t*>(newData) + newSize);
            }
            else
            {
                pendingData.data.resize(newSize);
            }

            pendingData.dirty |= ATTRIBUTES | DATA;

            return true;
        }

        bool BufferResource::setData(const void* newData, uint32_t newSize)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (!pendingData.dynamic)
            {
                return false;
            }

            if (newData && newSize)
            {
                pendingData.data.assign(static_cast<const uint8_t*>(newData),
                                        static_cast<const uint8_t*>(newData) + newSize);
            }
            else
            {
                pendingData.data.resize(newSize);
            }

            pendingData.dirty |= ATTRIBUTES | DATA;

            return true;
        }

        bool BufferResource::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            data.dirty |= pendingData.dirty;
            pendingData.dirty = 0;

            if (data.dirty & ATTRIBUTES)
            {
                data.usage = pendingData.usage;
                data.dynamic = pendingData.dynamic;
            }

            if (data.dirty & DATA)
            {
                data.data = std::move(pendingData.data);
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
