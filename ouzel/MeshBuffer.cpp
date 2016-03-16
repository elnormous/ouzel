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
        
        bool MeshBuffer::initFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes)
        {
            OUZEL_UNUSED(indices);
            OUZEL_UNUSED(indexCount);
            OUZEL_UNUSED(vertices);
            OUZEL_UNUSED(vertexCount);
            
            _indexCount = indexCount;
            _indexSize = indexSize;
            _dynamicIndexBuffer = dynamicIndexBuffer;
            
            _vertexCount = vertexCount;
            _vertexSize = vertexSize;
            _dynamicVertexBuffer = dynamicVertexBuffer;
            
            _vertexAttributes = vertexAttributes;
            
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
    } // namespace video
} // namespace ouzel
