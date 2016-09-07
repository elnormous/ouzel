// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "IndexBuffer.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"

namespace ouzel
{
    namespace graphics
    {
        IndexBuffer::IndexBuffer()
        {
        }

        IndexBuffer::~IndexBuffer()
        {
        }

        void IndexBuffer::free()
        {
            data.clear();
            uploadData.data.clear();

            ready = false;
        }

        bool IndexBuffer::init(bool newDynamic)
        {
            free();

            dynamic = newDynamic;

            return true;
        }

        bool IndexBuffer::initFromBuffer(const void* newIndices, uint32_t newIndexSize,
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

            dirty = INDEX_BUFFER_DIRTY | INDEX_SIZE_DIRTY;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool IndexBuffer::setData(const void* newIndices, uint32_t newIndexCount)
        {
            if (!dynamic)
            {
                return false;
            }

            indexCount = newIndexCount;

            data.assign(static_cast<const uint8_t*>(newIndices),
                        static_cast<const uint8_t*>(newIndices) + indexSize * indexCount);

            dirty |= INDEX_BUFFER_DIRTY;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool IndexBuffer::setIndexSize(uint32_t newIndexSize)
        {
            indexSize = newIndexSize;

            dirty |= INDEX_SIZE_DIRTY;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        void IndexBuffer::update()
        {
            uploadData.indexSize = indexSize;
            uploadData.dynamic = dynamic;
            
            if (!data.empty())
            {
                uploadData.data = std::move(data);
            }
        }

        bool IndexBuffer::upload()
        {
            ready = true;
            dirty = 0;
            return true;
        }
    }
}
