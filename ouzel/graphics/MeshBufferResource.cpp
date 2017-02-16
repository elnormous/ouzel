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

            indexSize = newIndexSize;
            indexBuffer = newIndexBuffer;
            vertexAttributes = newVertexAttributes;
            vertexBuffer = newVertexBuffer;
            vertexSize = 0;

            for (const VertexAttribute& vertexAttribute : vertexAttributes)
            {
                vertexSize += getDataTypeSize(vertexAttribute.dataType);
            }

            dirty |= INDEX_ATTRIBUTES | INDEX_BUFFER | VERTEX_ATTRIBUTES | VERTEX_BUFFER;

            return true;
        }

        bool MeshBufferResource::setIndexSize(uint32_t newIndexSize)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            indexSize = newIndexSize;

            dirty |= INDEX_ATTRIBUTES;

            return true;
        }

        bool MeshBufferResource::setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            vertexAttributes = newVertexAttributes;
            vertexSize = 0;

            for (const VertexAttribute& vertexAttribute : vertexAttributes)
            {
                vertexSize += getDataTypeSize(vertexAttribute.dataType);
            }

            dirty |= VERTEX_ATTRIBUTES;

            return true;
        }

        bool MeshBufferResource::setIndexBuffer(BufferResource* newIndexBuffer)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (newIndexBuffer && newIndexBuffer->getUsage() != Buffer::Usage::INDEX)
            {
                return false;
            }

            indexBuffer = newIndexBuffer;

            dirty |= INDEX_BUFFER;

            return true;
        }

        bool MeshBufferResource::setVertexBuffer(BufferResource* newVertexBuffer)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (newVertexBuffer && newVertexBuffer->getUsage() != Buffer::Usage::VERTEX)
            {
                return false;
            }

            vertexBuffer = newVertexBuffer;

            dirty |= VERTEX_BUFFER;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
