// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "IndexBufferResource.h"
#include "Renderer.h"

namespace ouzel
{
    namespace graphics
    {
        IndexBufferResource::IndexBufferResource()
        {
        }

        IndexBufferResource::~IndexBufferResource()
        {
        }

        void IndexBufferResource::free()
        {
            data.clear();
            currentData.data.clear();
        }

        bool IndexBufferResource::init(bool newDynamic)
        {
            free();

            dynamic = newDynamic;

            return true;
        }

        bool IndexBufferResource::initFromBuffer(const void* newIndices, uint32_t newIndexSize,
                                         uint32_t newIndexCount, bool newDynamic)
        {
            free();

            indexCount = newIndexCount;
            indexSize = newIndexSize;
            dynamic = newDynamic;

            if (newIndices && indexSize && indexCount)
            {
                data.assign(static_cast<const uint8_t*>(newIndices),
                            static_cast<const uint8_t*>(newIndices) + indexSize * indexCount);
            }

            update();

            return true;
        }

        bool IndexBufferResource::setData(const void* newIndices, uint32_t newIndexCount)
        {
            if (!dynamic)
            {
                return false;
            }

            indexCount = newIndexCount;

            data.assign(static_cast<const uint8_t*>(newIndices),
                        static_cast<const uint8_t*>(newIndices) + indexSize * indexCount);

            update();

            return true;
        }

        bool IndexBufferResource::setIndexSize(uint32_t newIndexSize)
        {
            indexSize = newIndexSize;

            update();

            return true;
        }

        void IndexBufferResource::update()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            currentData.indexSize = indexSize;
            currentData.dynamic = dynamic;
            currentData.data = std::move(data);

            dirty = true;
        }

        bool IndexBufferResource::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            dirty = false;
            uploadData = std::move(currentData);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
