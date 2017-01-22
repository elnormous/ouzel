// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "VertexBufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        VertexBufferResource::VertexBufferResource()
        {
        }

        VertexBufferResource::~VertexBufferResource()
        {
        }

        void VertexBufferResource::free()
        {
            data.clear();
            currentData.data.clear();
        }

        bool VertexBufferResource::init(bool newDynamic)
        {
            free();

            dynamic = newDynamic;

            return true;
        }

        bool VertexBufferResource::initFromBuffer(const void* newVertices, uint32_t newSize, bool newDynamic)
        {
            free();

            size = newSize;
            dynamic = newDynamic;

            if (newVertices && size)
            {
                data.assign(static_cast<const uint8_t*>(newVertices),
                            static_cast<const uint8_t*>(newVertices) + size);
            }

            update();

            return true;
        }

        bool VertexBufferResource::setData(const void* newVertices, uint32_t newSize)
        {
            if (!dynamic)
            {
                return false;
            }

            size = newSize;

            data.assign(static_cast<const uint8_t*>(newVertices),
                        static_cast<const uint8_t*>(newVertices) + size);

            update();

            return true;
        }

        void VertexBufferResource::update()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            currentData.dynamic = dynamic;
            currentData.data = std::move(data);

            dirty = true;
        }

        bool VertexBufferResource::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            dirty = false;
            uploadData = std::move(currentData);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
