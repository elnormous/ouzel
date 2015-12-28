// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBuffer.h"

namespace ouzel
{
    MeshBuffer::MeshBuffer(Renderer* renderer):
        _renderer(renderer)
    {
        
    }
    
    MeshBuffer::~MeshBuffer()
    {
        
    }
    
    bool MeshBuffer::initFromData(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices)
    {
        return true;
    }
    
    bool MeshBuffer::uploadIndices(const std::vector<uint16_t>& indices)
    {
        return true;
    }
    
    bool MeshBuffer::uploadVertices(const std::vector<Vertex>& vertices)
    {
        return true;
    }
}
