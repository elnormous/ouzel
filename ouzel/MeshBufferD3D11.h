// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <d3d11.h>
#include "MeshBuffer.h"

namespace ouzel
{
    class MeshBufferD3D11: public MeshBuffer
    {
    public:
        MeshBufferD3D11(Renderer* renderer);
        virtual ~MeshBufferD3D11();
        
        bool initFromData(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices);

        ID3D11Buffer* getIndexBuffer() const { return _indexBuffer; }
        ID3D11Buffer* getVertexBuffer() const { return _vertexBuffer; }

        UINT getIndexCount() const { return _indexCount; }

    protected:
        ID3D11Buffer* _indexBuffer = nullptr;
        ID3D11Buffer* _vertexBuffer = nullptr;
        UINT _indexCount = 0;
    };
}
