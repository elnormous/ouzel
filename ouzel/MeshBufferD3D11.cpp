// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferD3D11.h"
#include "Engine.h"
#include "RendererD3D11.h"
#include "Utils.h"

namespace ouzel
{
    MeshBufferD3D11::MeshBufferD3D11()
    {
        
    }
    
    MeshBufferD3D11::~MeshBufferD3D11()
    {
        if (_indexBuffer) _indexBuffer->Release();
        if (_vertexBuffer) _vertexBuffer->Release();
    }
    
    bool MeshBufferD3D11::initFromData(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices, bool dynamicIndexBuffer, bool dynamicVertexBuffer)
    {
        if (!MeshBuffer::initFromData(indices, vertices, dynamicIndexBuffer, dynamicVertexBuffer))
        {
            return false;
        }

        RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(Renderer::getInstance());

        D3D11_BUFFER_DESC indexBufferDesc;
        memset(&indexBufferDesc, 0, sizeof(indexBufferDesc));

        indexBufferDesc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint16_t));
        indexBufferDesc.Usage = _dynamicIndexBuffer ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = _dynamicIndexBuffer ? D3D11_CPU_ACCESS_WRITE : 0;

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

        vertexBufferDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));
        vertexBufferDesc.Usage = _dynamicVertexBuffer ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = _dynamicVertexBuffer ? D3D11_CPU_ACCESS_WRITE : 0;

        D3D11_SUBRESOURCE_DATA vertexBufferResourceData;
        memset(&vertexBufferResourceData, 0, sizeof(vertexBufferResourceData));
        vertexBufferResourceData.pSysMem = vertices.data();

        hr = rendererD3D11->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &_vertexBuffer);
        if (FAILED(hr) || !_vertexBuffer)
        {
            log("Failed to create D3D11 vertex buffer");
            return false;
        }

        _indexCount = static_cast<UINT>(indices.size());

        return true;
    }

    bool MeshBufferD3D11::uploadIndices(const std::vector<uint16_t>& indices)
    {
        // TODO: resize if needed
        return uploadData(_indexBuffer, indices.data(), indices.size() * sizeof(uint16_t));
    }

    bool MeshBufferD3D11::uploadVertices(const std::vector<Vertex>& vertices)
    {
        // TODO: resize if needed
        return uploadData(_vertexBuffer, vertices.data(), vertices.size() * sizeof(Vertex));
    }

    bool MeshBufferD3D11::uploadData(ID3D11Buffer* buffer, const void* data, uint32_t size)
    {
        RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(Renderer::getInstance());

        D3D11_MAPPED_SUBRESOURCE mappedSubResource;
        HRESULT hr = rendererD3D11->getContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
        if (FAILED(hr))
        {
            log("Failed to lock D3D11 buffer");
            return false;
        }

        memcpy(mappedSubResource.pData, data, size);

        rendererD3D11->getContext()->Unmap(buffer, 0);

        return true;
    }
}
