// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBuffer.h"
#include "Utils.h"

namespace ouzel
{
    namespace video
    {
        MeshBuffer::MeshBuffer()
        {

        }

        MeshBuffer::~MeshBuffer()
        {

        }

        bool MeshBuffer::initFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer)
        {
            OUZEL_UNUSED(indices);
            OUZEL_UNUSED(indexCount);
            OUZEL_UNUSED(vertices);
            OUZEL_UNUSED(vertexCount);

            _indexCount = indexCount;
            _indexSize = indexSize;
            _dynamicIndexBuffer = dynamicIndexBuffer;

            _vertexCount = vertexCount;
            _vertexAttributes = vertexAttributes;
            _dynamicVertexBuffer = dynamicVertexBuffer;
            updateVertexSize();

            return true;
        }

        bool MeshBuffer::uploadIndices(const void* indices, uint32_t indexCount)
        {
            OUZEL_UNUSED(indices);

            _indexCount = indexCount;

            if (!_dynamicIndexBuffer)
            {
                return false;
            }

            return true;
        }

        bool MeshBuffer::uploadVertices(const void* vertices, uint32_t vertexCount)
        {
            OUZEL_UNUSED(vertices);

            _vertexCount = vertexCount;

            if (!_dynamicVertexBuffer)
            {
                return false;
            }

            return true;
        }

        bool MeshBuffer::setVertexAttributes(uint32_t vertexAttributes)
        {
            _vertexAttributes = vertexAttributes;
            updateVertexSize();

            return true;
        }

        void MeshBuffer::updateVertexSize()
        {
            _vertexSize = 0;

            if (_vertexAttributes & VERTEX_POSITION)
            {
                _vertexSize += 3 * sizeof(float);
            }

            if (_vertexAttributes & VERTEX_COLOR)
            {
                _vertexSize += 4 * sizeof(uint8_t);
            }

            if (_vertexAttributes & VERTEX_NORMAL)
            {
                _vertexSize += 3 * sizeof(float);
            }

            if (_vertexAttributes & VERTEX_TEXCOORD0)
            {
                _vertexSize += 2 * sizeof(float);
            }

            if (_vertexAttributes & VERTEX_TEXCOORD1)
            {
                _vertexSize += 2 * sizeof(float);
            }
        }
    } // namespace video
} // namespace ouzel
