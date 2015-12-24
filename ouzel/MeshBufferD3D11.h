// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "MeshBuffer.h"

namespace ouzel
{
    class MeshBufferD3D11: public MeshBuffer
    {
    public:
        MeshBufferD3D11(Renderer* renderer);
        virtual ~MeshBufferD3D11();
        
        bool initFromData(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices);
    };
}
