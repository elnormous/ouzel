// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferInterface.hpp"
#include "BufferInterface.hpp"
#include "Vertex.hpp"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferInterface::MeshBufferInterface()
        {
        }

        MeshBufferInterface::~MeshBufferInterface()
        {
        }

        bool MeshBufferInterface::init(uint32_t newIndexSize, BufferInterface* newIndexBuffer,
                                       const std::vector<VertexAttribute>& newVertexAttributes, BufferInterface* newVertexBuffer)
        {
            indexSize = newIndexSize;
            indexBuffer = newIndexBuffer;
            vertexAttributes = newVertexAttributes;
            vertexBuffer = newVertexBuffer;
            vertexSize = 0;

            for (const VertexAttribute& vertexAttribute : vertexAttributes)
            {
                vertexSize += getDataTypeSize(vertexAttribute.dataType);
            }

            return true;
        }

        bool MeshBufferInterface::setIndexSize(uint32_t newIndexSize)
        {
            indexSize = newIndexSize;

            return true;
        }

        bool MeshBufferInterface::setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes)
        {
            vertexAttributes = newVertexAttributes;
            vertexSize = 0;

            for (const VertexAttribute& vertexAttribute : vertexAttributes)
            {
                vertexSize += getDataTypeSize(vertexAttribute.dataType);
            }

            return true;
        }

        bool MeshBufferInterface::setIndexBuffer(BufferInterface* newIndexBuffer)
        {
            if (newIndexBuffer && newIndexBuffer->getUsage() != Buffer::Usage::INDEX)
            {
                return false;
            }

            indexBuffer = newIndexBuffer;

            return true;
        }

        bool MeshBufferInterface::setVertexBuffer(BufferInterface* newVertexBuffer)
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
