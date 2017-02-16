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

            usage = newUsage;
            dynamic = newDynamic;

            dirty |= ATTRIBUTES;

            return true;
        }

        bool BufferResource::initFromBuffer(Buffer::Usage newUsage, const void* newData, uint32_t newSize, bool newDynamic)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            usage = newUsage;
            dynamic = newDynamic;

            if (newData && newSize)
            {
                data.assign(static_cast<const uint8_t*>(newData),
                            static_cast<const uint8_t*>(newData) + newSize);
            }
            else
            {
                data.resize(newSize);
            }

            dirty |= ATTRIBUTES | DATA;

            return true;
        }

        bool BufferResource::setData(const void* newData, uint32_t newSize)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (!dynamic)
            {
                return false;
            }

            if (newData && newSize)
            {
                data.assign(static_cast<const uint8_t*>(newData),
                            static_cast<const uint8_t*>(newData) + newSize);
            }
            else
            {
                data.resize(newSize);
            }

            dirty |= ATTRIBUTES | DATA;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
