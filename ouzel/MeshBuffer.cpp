// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBuffer.h"

namespace ouzel
{
    MeshBuffer::MeshBuffer()
    {
        
    }
    
    MeshBuffer::~MeshBuffer()
    {
        
    }
    
    bool MeshBuffer::initFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes)
    {
        _indexSize = indexSize;
        _dynamicIndexBuffer = dynamicIndexBuffer;
        
        _vertexSize = vertexSize;
        _dynamicVertexBuffer = dynamicVertexBuffer;
        
        _vertexAttributes = vertexAttributes;
        
        return true;
    }
    
    bool MeshBuffer::uploadIndices(const void* indices, uint32_t indexCount)
    {
        return true;
    }
    
    bool MeshBuffer::uploadVertices(const void* vertices, uint32_t vertexCount)
    {
        return true;
    }
}
