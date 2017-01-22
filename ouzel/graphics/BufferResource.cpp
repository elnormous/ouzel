// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BufferResource.h"
#include "Renderer.h"

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

        void BufferResource::free()
        {
            data.clear();
            currentData.data.clear();
        }

        bool BufferResource::init(Buffer::Usage newUsage, bool newDynamic)
        {
            free();

            usage = newUsage;
            dynamic = newDynamic;

            return true;
        }

        bool BufferResource::initFromBuffer(Buffer::Usage newUsage, const void* newIndices, uint32_t newSize, bool newDynamic)
        {
            free();

            usage = newUsage;
            dynamic = newDynamic;
            size = newSize;

            if (newIndices && size)
            {
                data.assign(static_cast<const uint8_t*>(newIndices),
                            static_cast<const uint8_t*>(newIndices) + size);
            }

            update();

            return true;
        }

        bool BufferResource::setData(const void* newIndices, uint32_t newSize)
        {
            if (!dynamic)
            {
                return false;
            }

            size = newSize;

            data.assign(static_cast<const uint8_t*>(newIndices),
                        static_cast<const uint8_t*>(newIndices) + size);

            update();

            return true;
        }

        void BufferResource::update()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            currentData.usage = usage;
            currentData.dynamic = dynamic;
            currentData.data = std::move(data);

            dirty = true;
        }

        bool BufferResource::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            dirty = false;
            uploadData = std::move(currentData);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
