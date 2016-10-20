// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "VertexBufferD3D11.h"
#include "RendererD3D11.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        VertexBufferD3D11::VertexBufferD3D11()
        {
        }

        VertexBufferD3D11::~VertexBufferD3D11()
        {
            if (buffer)
            {
                buffer->Release();
            }
        }

        void VertexBufferD3D11::free()
        {
            VertexBuffer::free();

            if (buffer)
            {
                buffer->Release();
                buffer = nullptr;
            }

            bufferSize = 0;
        }

        bool VertexBufferD3D11::upload()
        {
            if (uploadData.dirty)
            {
                RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

                if (uploadData.dirty & VERTEX_BUFFER_DIRTY)
                {
                    if (!uploadData.data.empty())
                    {
                        if (!buffer || uploadData.data.size() > bufferSize)
                        {
                            if (buffer) buffer->Release();

                            bufferSize = static_cast<UINT>(uploadData.data.size());

                            D3D11_BUFFER_DESC vertexBufferDesc;
                            memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));

                            vertexBufferDesc.ByteWidth = bufferSize;
                            vertexBufferDesc.Usage = uploadData.dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
                            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                            vertexBufferDesc.CPUAccessFlags = uploadData.dynamic ? D3D11_CPU_ACCESS_WRITE : 0;

                            D3D11_SUBRESOURCE_DATA vertexBufferResourceData;
                            memset(&vertexBufferResourceData, 0, sizeof(vertexBufferResourceData));
                            vertexBufferResourceData.pSysMem = uploadData.data.data();

                            HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &buffer);
                            if (FAILED(hr))
                            {
                                Log(Log::Level::ERR) << "Failed to create Direct3D 11 vertex buffer";
                                return false;
                            }
                        }
                        else
                        {
                            D3D11_MAPPED_SUBRESOURCE mappedSubResource;
                            HRESULT hr = rendererD3D11->getContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
                            if (FAILED(hr))
                            {
                                Log(Log::Level::ERR) << "Failed to lock Direct3D 11 buffer";
                                return false;
                            }

                            std::copy(uploadData.data.begin(), uploadData.data.end(), static_cast<uint8_t*>(mappedSubResource.pData));

                            rendererD3D11->getContext()->Unmap(buffer, 0);
                        }
                    }

                    uploadData.dirty &= ~VERTEX_BUFFER_DIRTY;
                }

                uploadData.dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
