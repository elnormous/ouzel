// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferD3D11.h"

namespace ouzel
{
    MeshBufferD3D11::MeshBufferD3D11(Renderer* renderer):
        MeshBuffer(renderer)
    {
        
    }
    
    MeshBufferD3D11::~MeshBufferD3D11()
    {

    }
    
    bool MeshBufferD3D11::initFromData(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices)
    {
        return true;
    }
}
