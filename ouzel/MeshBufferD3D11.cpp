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
    
    bool MeshBufferD3D11::initFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes)
    {
        if (!MeshBuffer::initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexSize, vertexCount, dynamicVertexBuffer, vertexAttributes))
        {
            return false;
        }

        if (!createIndexBuffer(indices, _indexSize * indexCount))
        {
            return false;
        }

        switch (_indexSize)
        {
            case 2: _indexFormat = DXGI_FORMAT_R16_UINT; break;
            case 4: _indexFormat = DXGI_FORMAT_R32_UINT; break;
            default: log("Invalid index size"); return false;
        }

        _indexCount = static_cast<UINT>(indexCount);

        uint32_t size = 0;

        if (vertexAttributes & VERTEX_POSITION) size += 3 * sizeof(float);
        if (vertexAttributes & VERTEX_COLOR) size += 4 * sizeof(uint8_t);
        if (vertexAttributes & VERTEX_NORMAL) size += 3 * sizeof(float);
        if (vertexAttributes & VERTEX_TEXCOORD0) size += 2 * sizeof(float);
        if (vertexAttributes & VERTEX_TEXCOORD1) size += 2 * sizeof(float);

        if (size != _vertexSize)
        {
            log("Invalid vertex size");
            return false;
        }

        if (!createVertexBuffer(vertices, _vertexSize * vertexCount))
        {
            return false;
        }
        
        return true;
    }

    bool MeshBufferD3D11::uploadIndices(const void* indices, uint32_t indexCount)
    {
        if (_indexSize * indexCount > _indexBufferSize)
        {
            if (_indexBuffer) _indexBuffer->Release();
            return createIndexBuffer(indices, _indexSize * indexCount);
        }
        else
        {
            return uploadData(_indexBuffer, indices, _indexSize * indexCount);
        }
    }

    bool MeshBufferD3D11::uploadVertices(const void* vertices, uint32_t vertexCount)
    {
        if (_vertexSize * vertexCount > _vertexBufferSize)
        {
            if (_vertexBuffer) _vertexBuffer->Release();
            return createVertexBuffer(vertices, _vertexSize * vertexCount);
        }
        else
        {
            return uploadData(_vertexBuffer, vertices, _vertexSize * vertexCount);
        }
    }

    bool MeshBufferD3D11::createIndexBuffer(const void* indices, uint32_t size)
    {
        RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(Renderer::getInstance());

        D3D11_BUFFER_DESC indexBufferDesc;
        memset(&indexBufferDesc, 0, sizeof(indexBufferDesc));

        indexBufferDesc.ByteWidth = static_cast<UINT>(size);
        indexBufferDesc.Usage = _dynamicIndexBuffer ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = _dynamicIndexBuffer ? D3D11_CPU_ACCESS_WRITE : 0;

        D3D11_SUBRESOURCE_DATA indexBufferResourceData;
        memset(&indexBufferResourceData, 0, sizeof(indexBufferResourceData));
        indexBufferResourceData.pSysMem = indices;

        HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&indexBufferDesc, &indexBufferResourceData, &_indexBuffer);
        if (FAILED(hr) || !_indexBuffer)
        {
            log("Failed to create D3D11 index buffer");
            return false;
        }

        _indexBufferSize = static_cast<uint32_t>(size);

        return true;
    }

    bool MeshBufferD3D11::createVertexBuffer(const void* vertices, uint32_t size)
    {
        RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(Renderer::getInstance());

        D3D11_BUFFER_DESC vertexBufferDesc;
        memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));

        vertexBufferDesc.ByteWidth = static_cast<UINT>(size);
        vertexBufferDesc.Usage = _dynamicVertexBuffer ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = _dynamicVertexBuffer ? D3D11_CPU_ACCESS_WRITE : 0;

        D3D11_SUBRESOURCE_DATA vertexBufferResourceData;
        memset(&vertexBufferResourceData, 0, sizeof(vertexBufferResourceData));
        vertexBufferResourceData.pSysMem = vertices;

        HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &_vertexBuffer);
        if (FAILED(hr) || !_vertexBuffer)
        {
            log("Failed to create D3D11 vertex buffer");
            return false;
        }

        _vertexBufferSize = static_cast<uint32_t>(size);

        return true;
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
