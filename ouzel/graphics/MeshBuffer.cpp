// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBuffer.h"
#include "core/Engine.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBuffer::MeshBuffer()
        {

        }

        MeshBuffer::~MeshBuffer()
        {

        }

        void MeshBuffer::free()
        {
            indexData.clear();
            vertexData.clear();
            uploadData.indexData.clear();
            uploadData.vertexData.clear();
            
            ready = false;
        }

        bool MeshBuffer::init(bool newDynamicIndexBuffer, bool newDynamicVertexBuffer)
        {
            free();
            
            dynamicIndexBuffer = newDynamicIndexBuffer;
            dynamicVertexBuffer = newDynamicVertexBuffer;

            return true;
        }

        bool MeshBuffer::initFromBuffer(const void* newIndices, uint32_t newIndexSize,
                                        uint32_t newIndexCount, bool newDynamicIndexBuffer,
                                        const void* newVertices, uint32_t newVertexAttributes,
                                        uint32_t newVertexCount, bool newDynamicVertexBuffer)
        {
            free();

            indexCount = newIndexCount;
            indexSize = newIndexSize;
            dynamicIndexBuffer = newDynamicIndexBuffer;

            vertexCount = newVertexCount;
            vertexAttributes = newVertexAttributes;
            dynamicVertexBuffer = newDynamicVertexBuffer;
            updateVertexSize();

            if (newIndices && indexSize && indexCount)
            {
                indexData.assign(static_cast<const uint8_t*>(newIndices),
                                      static_cast<const uint8_t*>(newIndices) + indexSize * indexCount);
            }

            if (newVertices && vertexSize && vertexCount)
            {
                vertexData.assign(static_cast<const uint8_t*>(newVertices),
                                  static_cast<const uint8_t*>(newVertices) + vertexSize * vertexCount);
            }

            dirty = INDEX_BUFFER_DIRTY | VERTEX_BUFFER_DIRTY | INDEX_SIZE_DIRTY | VERTEX_ATTRIBUTES_DIRTY;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBuffer::setIndices(const void* newIndices, uint32_t newIndexCount)
        {
            if (!dynamicIndexBuffer)
            {
                return false;
            }

            indexCount = newIndexCount;

            indexData.assign(static_cast<const uint8_t*>(newIndices),
                             static_cast<const uint8_t*>(newIndices) + indexSize * indexCount);

            dirty |= INDEX_BUFFER_DIRTY;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBuffer::setVertices(const void* newVertices, uint32_t newVertexCount)
        {
            if (!dynamicVertexBuffer)
            {
                return false;
            }

            vertexCount = newVertexCount;

            vertexData.assign(static_cast<const uint8_t*>(newVertices),
                              static_cast<const uint8_t*>(newVertices) + vertexSize * vertexCount);

            dirty |= VERTEX_BUFFER_DIRTY;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBuffer::setIndexSize(uint32_t newIndexSize)
        {
            indexSize = newIndexSize;

            dirty |= INDEX_SIZE_DIRTY;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBuffer::setVertexAttributes(uint32_t newVertexAttributes)
        {
            vertexAttributes = newVertexAttributes;
            updateVertexSize();

            dirty |= VERTEX_ATTRIBUTES_DIRTY;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        void MeshBuffer::updateVertexSize()
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

        bool MeshBuffer::update()
        {
            uploadData.indexSize = indexSize;
            uploadData.vertexSize = vertexSize;
            uploadData.vertexAttributes = vertexAttributes;
            uploadData.dynamicIndexBuffer = dynamicIndexBuffer;
            uploadData.dynamicVertexBuffer = dynamicVertexBuffer;

            if (!indexData.empty())
            {
                uploadData.indexData = std::move(indexData);
            }

            if (!vertexData.empty())
            {
                uploadData.vertexData = std::move(vertexData);
            }
            
            return true;
        }
    } // namespace graphics
} // namespace ouzel
