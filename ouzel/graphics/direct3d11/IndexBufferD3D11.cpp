// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "IndexBufferD3D11.h"
#include "RendererD3D11.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        IndexBufferD3D11::IndexBufferD3D11()
        {
        }

        IndexBufferD3D11::~IndexBufferD3D11()
        {
            if (buffer)
            {
                buffer->Release();
            }
        }

        void IndexBufferD3D11::free()
        {
            IndexBuffer::free();

            if (buffer)
            {
                buffer->Release();
                buffer = nullptr;
            }

            bufferSize = 0;
        }

        bool IndexBufferD3D11::upload()
        {
            if (uploadData.dirty)
            {
                RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

                if (uploadData.dirty & INDEX_SIZE_DIRTY)
                {
                    switch (uploadData.indexSize)
                    {
                    case 2:
                        format = DXGI_FORMAT_R16_UINT;
                        break;
                    case 4:
                        format = DXGI_FORMAT_R32_UINT;
                        break;
                    default:
                        format = DXGI_FORMAT_UNKNOWN;
                        Log(Log::Level::ERR) << "Invalid index size";
                        return false;
                    }

                    uploadData.dirty &= ~INDEX_SIZE_DIRTY;
                }

                if (uploadData.dirty & INDEX_BUFFER_DIRTY)
                {
                    if (!uploadData.data.empty())
                    {
                        if (!buffer || uploadData.data.size() > bufferSize)
                        {
                            if (buffer) buffer->Release();

                            bufferSize = static_cast<UINT>(uploadData.data.size());

                            D3D11_BUFFER_DESC indexBufferDesc;
                            memset(&indexBufferDesc, 0, sizeof(indexBufferDesc));

                            indexBufferDesc.ByteWidth = bufferSize;
                            indexBufferDesc.Usage = uploadData.dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
                            indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                            indexBufferDesc.CPUAccessFlags = uploadData.dynamic ? D3D11_CPU_ACCESS_WRITE : 0;

                            D3D11_SUBRESOURCE_DATA indexBufferResourceData;
                            memset(&indexBufferResourceData, 0, sizeof(indexBufferResourceData));
                            indexBufferResourceData.pSysMem = uploadData.data.data();

                            HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&indexBufferDesc, &indexBufferResourceData, &buffer);
                            if (FAILED(hr))
                            {
                                Log(Log::Level::ERR) << "Failed to create Direct3D 11 index buffer";
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

                            memcpy(mappedSubResource.pData, uploadData.data.data(), uploadData.data.size());

                            rendererD3D11->getContext()->Unmap(buffer, 0);
                        }
                    }

                    uploadData.dirty &= ~INDEX_BUFFER_DIRTY;
                }

                uploadData.dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
