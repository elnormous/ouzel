// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferD3D11.h"
#include "RendererD3D11.h"
#include "Utils.h"

namespace ouzel
{
    MeshBufferD3D11::MeshBufferD3D11(Renderer* renderer):
        MeshBuffer(renderer)
    {
        
    }
    
    MeshBufferD3D11::~MeshBufferD3D11()
    {
        if (_indexBuffer) _indexBuffer->Release();
        if (_vertexBuffer) _vertexBuffer->Release();
    }
    
    bool MeshBufferD3D11::initFromData(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices)
    {
        if (!MeshBuffer::initFromData(indices, vertices))
        {
            return false;
        }

        RendererD3D11* rendererD3D11 = (RendererD3D11*)_renderer;

        D3D11_BUFFER_DESC indexBufferDesc;
        memset(&indexBufferDesc, 0, sizeof(indexBufferDesc));

        indexBufferDesc.ByteWidth = (UINT)indices.size() * sizeof(uint16_t);
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA indexBufferResourceData;
        memset(&indexBufferResourceData, 0, sizeof(indexBufferResourceData));
        indexBufferResourceData.pSysMem = indices.data();

        HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&indexBufferDesc, &indexBufferResourceData, &_indexBuffer);
        if (FAILED(hr) || !_indexBuffer)
        {
            log("Failed to create D3D11 index buffer");
            return false;
        }

        D3D11_BUFFER_DESC vertexBufferDesc;
        memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));

        vertexBufferDesc.ByteWidth = (UINT)vertices.size() * sizeof(Vertex);
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA vertexBufferResourceData;
        memset(&vertexBufferResourceData, 0, sizeof(vertexBufferResourceData));
        vertexBufferResourceData.pSysMem = vertices.data();

        hr = rendererD3D11->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &_vertexBuffer);
        if (FAILED(hr) || !_vertexBuffer)
        {
            log("Failed to create D3D11 vertex buffer");
            return false;
        }

        _indexCount = (UINT)indices.size();

        return true;
    }
}
