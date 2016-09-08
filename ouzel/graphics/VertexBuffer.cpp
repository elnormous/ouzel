// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "VertexBuffer.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"

namespace ouzel
{
    namespace graphics
    {
        VertexBuffer::VertexBuffer()
        {
        }

        VertexBuffer::~VertexBuffer()
        {
        }

        void VertexBuffer::free()
        {
            data.clear();
            uploadData.data.clear();

            ready = false;
        }

        bool VertexBuffer::init(bool newDynamic)
        {
            free();

            dynamic = newDynamic;

            return true;
        }

        bool VertexBuffer::initFromBuffer(const void* newVertices, uint32_t newVertexAttributes,
                                          uint32_t newVertexCount, bool newDynamic)
        {
            free();

            vertexAttributes = newVertexAttributes;
            vertexCount = newVertexCount;
            dynamic = newDynamic;

            updateVertexSize();

            if (newVertices && vertexSize && vertexCount)
            {
                data.assign(static_cast<const uint8_t*>(newVertices),
                            static_cast<const uint8_t*>(newVertices) + vertexSize * vertexCount);
            }

            dirty = VERTEX_BUFFER_DIRTY | VERTEX_ATTRIBUTES_DIRTY;
            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool VertexBuffer::setData(const void* newVertices, uint32_t newVertexCount)
        {
            if (!dynamic)
            {
                return false;
            }

            vertexCount = newVertexCount;

            data.assign(static_cast<const uint8_t*>(newVertices),
                        static_cast<const uint8_t*>(newVertices) + vertexSize * vertexCount);

            dirty |= VERTEX_BUFFER_DIRTY;
            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());
            
            return true;
        }

        bool VertexBuffer::setVertexAttributes(uint32_t newVertexAttributes)
        {
            vertexAttributes = newVertexAttributes;
            updateVertexSize();

            dirty |= VERTEX_ATTRIBUTES_DIRTY;
            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        void VertexBuffer::updateVertexSize()
        {
            vertexSize = 0;

            if (vertexAttributes & VERTEX_POSITION)
            {
                vertexSize += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_COLOR)
            {
                vertexSize += 4 * sizeof(uint8_t);
            }

            if (vertexAttributes & VERTEX_NORMAL)
            {
                vertexSize += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD0)
            {
                vertexSize += 2 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD1)
            {
                vertexSize += 2 * sizeof(float);
            }
        }

        void VertexBuffer::update()
        {
            uploadData.vertexSize = vertexSize;
            uploadData.vertexAttributes = vertexAttributes;
            uploadData.dynamic = dynamic;

            if (!data.empty())
            {
                uploadData.data = std::move(data);
            }
        }
    } // namespace graphics
} // namespace ouzel
