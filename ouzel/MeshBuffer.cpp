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
    
    bool MeshBuffer::initFromData(const std::vector<uint16_t>& indices, const std::vector<VertexPCT>& vertices, bool dynamicIndexBuffer, bool dynamicVertexBuffer)
    {
        _dynamicIndexBuffer = dynamicIndexBuffer;
        _dynamicVertexBuffer = dynamicVertexBuffer;
        
        return true;
    }
    
    bool MeshBuffer::uploadIndices(const std::vector<uint16_t>& indices)
    {
        return true;
    }
    
    bool MeshBuffer::uploadVertices(const std::vector<VertexPCT>& vertices)
    {
        return true;
    }
}
