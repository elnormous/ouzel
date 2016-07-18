// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBuffer.h"
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
            ready = false;
        }

        bool MeshBuffer::init(bool newDynamicIndexBuffer, bool newDynamicVertexBuffer)
        {
            dynamicIndexBuffer = newDynamicIndexBuffer;
            dynamicVertexBuffer = newDynamicVertexBuffer;

            return true;
        }

        bool MeshBuffer::initFromBuffer(const void*, uint32_t newIndexSize,
                                        uint32_t newIndexCount, bool newDynamicIndexBuffer,
                                        const void*, uint32_t newVertexAttributes,
                                        uint32_t newVertexCount, bool newDynamicVertexBuffer)
        {
            indexCount = newIndexCount;
            indexSize = newIndexSize;
            dynamicIndexBuffer = newDynamicIndexBuffer;

            vertexCount = newVertexCount;
            vertexAttributes = newVertexAttributes;
            dynamicVertexBuffer = newDynamicVertexBuffer;
            updateVertexSize();

            ready = true;

            return true;
        }

        bool MeshBuffer::uploadIndices(const void*, uint32_t newIndexCount)
        {
            if (!dynamicIndexBuffer)
            {
                return false;
            }

            indexCount = newIndexCount;

            return true;
        }

        bool MeshBuffer::uploadVertices(const void*, uint32_t newVertexCount)
        {

            if (!dynamicVertexBuffer)
            {
                return false;
            }

            vertexCount = newVertexCount;

            return true;
        }

        bool MeshBuffer::setIndexSize(uint32_t newIndexSize)
        {
            indexSize = newIndexSize;
            return true;
        }

        bool MeshBuffer::setVertexAttributes(uint32_t newVertexAttributes)
        {
            vertexAttributes = newVertexAttributes;
            updateVertexSize();

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
    } // namespace graphics
} // namespace ouzel
