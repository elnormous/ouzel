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
                                      const std::vector<VertexAttribute>& newVertexAttributes, BufferResource* newVertexBuffer)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            pendingData.indexSize = newIndexSize;
            pendingData.indexBuffer = newIndexBuffer;
            pendingData.vertexAttributes = newVertexAttributes;
            pendingData.vertexBuffer = newVertexBuffer;
            pendingData.vertexSize = 0;

            for (const VertexAttribute& vertexAttribute : pendingData.vertexAttributes)
            {
                pendingData.vertexSize += getDataTypeSize(vertexAttribute.dataType);
            }

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

        bool MeshBufferResource::setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            pendingData.vertexAttributes = newVertexAttributes;
            pendingData.vertexSize = 0;

            for (const VertexAttribute& vertexAttribute : pendingData.vertexAttributes)
            {
                pendingData.vertexSize += getDataTypeSize(vertexAttribute.dataType);
            }

            pendingData.dirty |= VERTEX_ATTRIBUTES;

            return true;
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
