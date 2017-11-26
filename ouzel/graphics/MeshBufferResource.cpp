// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferResource.hpp"
#include "BufferResource.hpp"
#include "Vertex.hpp"

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
                                      const std::vector<Vertex::Attribute>& newVertexAttributes, BufferResource* newVertexBuffer)
        {
            indexSize = newIndexSize;
            indexBuffer = newIndexBuffer;
            vertexAttributes = newVertexAttributes;
            vertexBuffer = newVertexBuffer;
            vertexSize = 0;

            for (const Vertex::Attribute& vertexAttribute : vertexAttributes)
            {
                vertexSize += getDataTypeSize(vertexAttribute.dataType);
            }

            return true;
        }

        bool MeshBufferResource::setIndexSize(uint32_t newIndexSize)
        {
            indexSize = newIndexSize;

            return true;
        }

        bool MeshBufferResource::setVertexAttributes(const std::vector<Vertex::Attribute>& newVertexAttributes)
        {
            vertexAttributes = newVertexAttributes;
            vertexSize = 0;

            for (const Vertex::Attribute& vertexAttribute : vertexAttributes)
            {
                vertexSize += getDataTypeSize(vertexAttribute.dataType);
            }

            return true;
        }

        bool MeshBufferResource::setIndexBuffer(BufferResource* newIndexBuffer)
        {
            if (newIndexBuffer && newIndexBuffer->getUsage() != Buffer::Usage::INDEX)
            {
                return false;
            }

            indexBuffer = newIndexBuffer;

            return true;
        }

        bool MeshBufferResource::setVertexBuffer(BufferResource* newVertexBuffer)
        {
            if (newVertexBuffer && newVertexBuffer->getUsage() != Buffer::Usage::VERTEX)
            {
                return false;
            }

            vertexBuffer = newVertexBuffer;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
