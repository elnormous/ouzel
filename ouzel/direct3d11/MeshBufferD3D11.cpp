// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferD3D11.h"
#include "Engine.h"
#include "RendererD3D11.h"
#include "Utils.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferD3D11::MeshBufferD3D11()
        {

        }

        MeshBufferD3D11::~MeshBufferD3D11()
        {
            destroy();
        }

        void MeshBufferD3D11::destroy()
        {
            if (indexBuffer)
            {
                indexBuffer->Release();
                indexBuffer = nullptr;
            }

            if (vertexBuffer)
            {
                vertexBuffer->Release();
                vertexBuffer = nullptr;
            }
        }

        bool MeshBufferD3D11::initFromData(const void* newIndices, uint32_t newIndexSize, uint32_t newIndexCount, bool newDynamicIndexBuffer, const void* newVertices, uint32_t newVertexAttributes, uint32_t newVertexCount, bool newDynamicVertexBuffer)
        {
            if (!MeshBuffer::initFromData(newIndices, newIndexSize, newIndexCount, newDynamicIndexBuffer, newVertices, newVertexAttributes, newVertexCount, newDynamicVertexBuffer))
            {
                return false;
            }

            destroy();

            if (!createIndexBuffer(newIndices, indexSize * indexCount))
            {
                return false;
            }

            if (!updateIndexFormat())
            {
                return false;
            }

            if (!createVertexBuffer(newVertices, vertexSize * vertexCount))
            {
                return false;
            }

            return true;
        }

        bool MeshBufferD3D11::setIndexSize(uint32_t indexSize)
        {
            if (!MeshBuffer::setIndexSize(indexSize))
            {
                return false;
            }

            return updateIndexFormat();
        }

        bool MeshBufferD3D11::uploadIndices(const void* indices, uint32_t indexCount)
        {
            if (!MeshBuffer::uploadIndices(indices, indexCount))
            {
                return false;
            }

            if (indexSize * indexCount > indexBufferSize)
            {
				if (indexBuffer)
				{
					indexBuffer->Release();
					indexBuffer = nullptr;
				}
                return createIndexBuffer(indices, indexSize * indexCount);
            }
            else
            {
                return uploadData(indexBuffer, indices, indexSize * indexCount);
            }
        }

        bool MeshBufferD3D11::uploadVertices(const void* vertices, uint32_t vertexCount)
        {
            if (!MeshBuffer::uploadVertices(vertices, vertexCount))
            {
                return false;
            }

            if (vertexSize * vertexCount > vertexBufferSize)
            {
				if (vertexBuffer)
				{
					vertexBuffer->Release();
					vertexBuffer = nullptr;
				}
                return createVertexBuffer(vertices, vertexSize * vertexCount);
            }
            else
            {
                return uploadData(vertexBuffer, vertices, vertexSize * vertexCount);
            }
        }

        bool MeshBufferD3D11::updateIndexFormat()
        {
            switch (indexSize)
            {
                case 2: indexFormat = DXGI_FORMAT_R16_UINT; break;
                case 4: indexFormat = DXGI_FORMAT_R32_UINT; break;
                default: log("Invalid index size"); return false;
            }

            return true;
        }

        bool MeshBufferD3D11::createIndexBuffer(const void* indices, uint32_t size)
        {
            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            D3D11_BUFFER_DESC indexBufferDesc;
            memset(&indexBufferDesc, 0, sizeof(indexBufferDesc));

            indexBufferDesc.ByteWidth = static_cast<UINT>(size);
            indexBufferDesc.Usage = dynamicIndexBuffer ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
            indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            indexBufferDesc.CPUAccessFlags = dynamicIndexBuffer ? D3D11_CPU_ACCESS_WRITE : 0;

            D3D11_SUBRESOURCE_DATA indexBufferResourceData;
            memset(&indexBufferResourceData, 0, sizeof(indexBufferResourceData));
            indexBufferResourceData.pSysMem = indices;

            HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&indexBufferDesc, &indexBufferResourceData, &indexBuffer);
            if (FAILED(hr) || !indexBuffer)
            {
                log("Failed to create D3D11 index buffer");
                return false;
            }

            indexBufferSize = size;

            return true;
        }

        bool MeshBufferD3D11::createVertexBuffer(const void* vertices, uint32_t size)
        {
            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            D3D11_BUFFER_DESC vertexBufferDesc;
            memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));

            vertexBufferDesc.ByteWidth = static_cast<UINT>(size);
            vertexBufferDesc.Usage = dynamicVertexBuffer ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vertexBufferDesc.CPUAccessFlags = dynamicVertexBuffer ? D3D11_CPU_ACCESS_WRITE : 0;

            D3D11_SUBRESOURCE_DATA vertexBufferResourceData;
            memset(&vertexBufferResourceData, 0, sizeof(vertexBufferResourceData));
            vertexBufferResourceData.pSysMem = vertices;

            HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &vertexBuffer);
            if (FAILED(hr) || !vertexBuffer)
            {
                log("Failed to create D3D11 vertex buffer");
                return false;
            }

            vertexBufferSize = size;

            return true;
        }

        bool MeshBufferD3D11::uploadData(ID3D11Buffer* buffer, const void* data, uint32_t size)
        {
            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

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
    } // namespace graphics
} // namespace ouzel
