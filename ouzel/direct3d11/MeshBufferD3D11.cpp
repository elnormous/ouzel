// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferD3D11.h"
#include "core/Engine.h"
#include "RendererD3D11.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferD3D11::MeshBufferD3D11():
            indexBufferDirty(false), vertexBufferDirty(false)
        {

        }

        MeshBufferD3D11::~MeshBufferD3D11()
        {
            if (indexBuffer)
            {
                indexBuffer->Release();
            }

            if (vertexBuffer)
            {
                vertexBuffer->Release();
            }
        }

        void MeshBufferD3D11::free()
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            MeshBuffer::free();

            indexData.clear();
            vertexData.clear();

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

        bool MeshBufferD3D11::init(bool newDynamicIndexBuffer, bool newDynamicVertexBuffer)
        {
            free();

            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::init(newDynamicIndexBuffer, newDynamicVertexBuffer))
            {
                return false;
            }

            indexBufferDirty = true;
            vertexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferD3D11::initFromBuffer(const void* newIndices, uint32_t newIndexSize,
                                             uint32_t newIndexCount, bool newDynamicIndexBuffer,
                                             const void* newVertices, uint32_t newVertexAttributes,
                                             uint32_t newVertexCount, bool newDynamicVertexBuffer)
        {
            free();

            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::initFromBuffer(newIndices, newIndexSize, newIndexCount, newDynamicIndexBuffer, newVertices, newVertexAttributes, newVertexCount, newDynamicVertexBuffer))
            {
                return false;
            }

            if (newIndices && indexSize && indexCount)
            {
                indexData.assign(static_cast<const uint8_t*>(newIndices),
                                 static_cast<const uint8_t*>(newIndices) + indexSize * indexCount);
            }

            if (newVertices && vertexSize && vertexCount)
            {
                vertexData.assign(static_cast<const uint8_t*>(newVertices),
                                  static_cast<const uint8_t*>(newVertices) + vertexSize * vertexCount);
            }

            indexBufferDirty = true;
            vertexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferD3D11::setIndexSize(uint32_t indexSize)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::setIndexSize(indexSize))
            {
                return false;
            }

            indexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferD3D11::uploadIndices(const void* newIndices, uint32_t newIndexCount)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::uploadIndices(newIndices, newIndexCount))
            {
                return false;
            }

            indexData.assign(static_cast<const uint8_t*>(newIndices),
                             static_cast<const uint8_t*>(newIndices) + indexSize * indexCount);

            indexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferD3D11::uploadVertices(const void* newVertices, uint32_t newVertexCount)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::uploadVertices(newVertices, newVertexCount))
            {
                return false;
            }

            vertexData.assign(static_cast<const uint8_t*>(newVertices),
                              static_cast<const uint8_t*>(newVertices) + vertexSize * vertexCount);

            vertexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferD3D11::uploadData(ID3D11Buffer* buffer, const std::vector<uint8_t>& data)
        {
            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            D3D11_MAPPED_SUBRESOURCE mappedSubResource;
            HRESULT hr = rendererD3D11->getContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
            if (FAILED(hr))
            {
                log("Failed to lock Direct3D 11 buffer");
                return false;
            }

            memcpy(mappedSubResource.pData, data.data(), data.size());

            rendererD3D11->getContext()->Unmap(buffer, 0);

            return true;
        }

        bool MeshBufferD3D11::update()
        {
            if (indexBufferDirty || vertexBufferDirty)
            {
                std::vector<uint8_t> localIndexData;
                std::vector<uint8_t> localVertexData;

                {
                    std::lock_guard<std::mutex> lock(dataMutex);

                    if (indexBufferDirty)
                    {
                        localIndexData = indexData;
                        switch (indexSize)
                        {
                            case 2: indexFormat = DXGI_FORMAT_R16_UINT; break;
                            case 4: indexFormat = DXGI_FORMAT_R32_UINT; break;
                            default: log("Invalid index size"); return false;
                        }
                    }

                    if (vertexBufferDirty)
                    {
                        localVertexData = vertexData;
                    }
                }

                std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

                if (indexBufferDirty)
                {
                    if (!localIndexData.empty())
                    {
                        if (!indexBuffer || localIndexData.size() > indexBufferSize)
                        {
                            if (indexBuffer) indexBuffer->Release();

                            indexBufferSize = static_cast<UINT>(localIndexData.size());

                            D3D11_BUFFER_DESC indexBufferDesc;
                            memset(&indexBufferDesc, 0, sizeof(indexBufferDesc));

                            indexBufferDesc.ByteWidth = indexBufferSize;
                            indexBufferDesc.Usage = dynamicIndexBuffer ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
                            indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                            indexBufferDesc.CPUAccessFlags = dynamicIndexBuffer ? D3D11_CPU_ACCESS_WRITE : 0;

                            D3D11_SUBRESOURCE_DATA indexBufferResourceData;
                            memset(&indexBufferResourceData, 0, sizeof(indexBufferResourceData));
                            indexBufferResourceData.pSysMem = localIndexData.data();

                            HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&indexBufferDesc, &indexBufferResourceData, &indexBuffer);
                            if (FAILED(hr))
                            {
                                log("Failed to create Direct3D 11 index buffer");
                                return false;
                            }
                        }
                        else
                        {
                            if (!uploadData(indexBuffer, localIndexData))
                            {
                                return false;
                            }
                        }
                    }

                    indexBufferDirty = false;
                }

                if (vertexBufferDirty)
                {
                    if (!localVertexData.empty())
                    {
                        if (!vertexBuffer || localVertexData.size() > vertexBufferSize)
                        {
                            if (vertexBuffer) vertexBuffer->Release();

                            vertexBufferSize = static_cast<UINT>(localVertexData.size());

                            D3D11_BUFFER_DESC vertexBufferDesc;
                            memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));

                            vertexBufferDesc.ByteWidth = vertexBufferSize;
                            vertexBufferDesc.Usage = dynamicVertexBuffer ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
                            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                            vertexBufferDesc.CPUAccessFlags = dynamicVertexBuffer ? D3D11_CPU_ACCESS_WRITE : 0;

                            D3D11_SUBRESOURCE_DATA vertexBufferResourceData;
                            memset(&vertexBufferResourceData, 0, sizeof(vertexBufferResourceData));
                            vertexBufferResourceData.pSysMem = localVertexData.data();

                            HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &vertexBuffer);
                            if (FAILED(hr))
                            {
                                log("Failed to create Direct3D 11 vertex buffer");
                                return false;
                            }
                        }
                        else
                        {
                            if (!uploadData(vertexBuffer, localVertexData))
                            {
                                return false;
                            }
                        }
                    }

                    vertexBufferDirty = false;
                }

                ready = (indexBuffer && vertexBuffer);
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
