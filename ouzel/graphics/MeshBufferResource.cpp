// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferResource.h"
#include "BufferResource.h"
#include "Vertex.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferResource::MeshBufferResource()
        {
        }

        MeshBufferResource::~MeshBufferResource()
        {
        }

        bool MeshBufferResource::init(uint32_t newIndexSize, BufferResource* newIndexBuffer,
                                      uint32_t newVertexAttributes, BufferResource* newVertexBuffer)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            pendingData.indexSize = newIndexSize;
            pendingData.indexBuffer = newIndexBuffer;
            pendingData.vertexAttributes = newVertexAttributes;
            pendingData.vertexBuffer = newVertexBuffer;
            updateVertexSize();

            pendingData.dirty |= INDEX_ATTRIBUTES | INDEX_BUFFER | VERTEX_ATTRIBUTES | VERTEX_BUFFER;

            return true;
        }

        bool MeshBufferResource::setIndexSize(uint32_t newIndexSize)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            pendingData.indexSize = newIndexSize;

            pendingData.dirty |= INDEX_ATTRIBUTES;

            return true;
        }

        bool MeshBufferResource::setVertexAttributes(uint32_t newVertexAttributes)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            pendingData.vertexAttributes = newVertexAttributes;
            updateVertexSize();

            pendingData.dirty |= VERTEX_ATTRIBUTES;

            return true;
        }

        void MeshBufferResource::updateVertexSize()
        {
            pendingData.vertexSize = 0;

            if (pendingData.vertexAttributes & VERTEX_POSITION)
            {
                pendingData.vertexSize += 3 * sizeof(float);
            }

            if (pendingData.vertexAttributes & VERTEX_COLOR)
            {
                pendingData.vertexSize += 4 * sizeof(uint8_t);
            }

            if (pendingData.vertexAttributes & VERTEX_NORMAL)
            {
                pendingData.vertexSize += 3 * sizeof(float);
            }

            if (pendingData.vertexAttributes & VERTEX_TEXCOORD0)
            {
                pendingData.vertexSize += 2 * sizeof(float);
            }

            if (pendingData.vertexAttributes & VERTEX_TEXCOORD1)
            {
                pendingData.vertexSize += 2 * sizeof(float);
            }
        }

        bool MeshBufferResource::setIndexBuffer(BufferResource* newIndexBuffer)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (newIndexBuffer && newIndexBuffer->getUsage() != Buffer::Usage::INDEX)
            {
                return false;
            }

            pendingData.indexBuffer = newIndexBuffer;

            pendingData.dirty |= INDEX_BUFFER;

            return true;
        }

        bool MeshBufferResource::setVertexBuffer(BufferResource* newVertexBuffer)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (newVertexBuffer && newVertexBuffer->getUsage() != Buffer::Usage::VERTEX)
            {
                return false;
            }

            pendingData.vertexBuffer = newVertexBuffer;
            
            pendingData.dirty |= VERTEX_BUFFER;

            return true;
        }

        bool MeshBufferResource::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            data.dirty |= pendingData.dirty;
            pendingData.dirty = 0;

            if (data.dirty)
            {
                data.indexSize = pendingData.indexSize;
                data.indexBuffer = pendingData.indexBuffer;
                data.vertexAttributes = pendingData.vertexAttributes;
                data.vertexSize = pendingData.vertexSize;
                data.vertexBuffer = pendingData.vertexBuffer;
            }
            
            return true;
        }
    } // namespace graphics
} // namespace ouzel
