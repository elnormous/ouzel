// Copyright (C) 2017 Elviss Strazdins
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
            IndexBufferResource::free();

            if (buffer)
            {
                buffer->Release();
                buffer = nullptr;
            }

            bufferSize = 0;
        }

        bool IndexBufferD3D11::upload()
        {
            if (!IndexBufferResource::upload())
            {
                return false;
            }

            RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

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

            if (!uploadData.data.empty())
            {
                if (!buffer || uploadData.data.size() > bufferSize)
                {
                    if (buffer) buffer->Release();

                    bufferSize = static_cast<UINT>(uploadData.data.size());

                    D3D11_BUFFER_DESC indexBufferDesc;
                    indexBufferDesc.ByteWidth = bufferSize;
                    indexBufferDesc.Usage = uploadData.dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
                    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                    indexBufferDesc.CPUAccessFlags = uploadData.dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
                    indexBufferDesc.MiscFlags = 0;
                    indexBufferDesc.StructureByteStride = 0;

                    D3D11_SUBRESOURCE_DATA indexBufferResourceData;
                    indexBufferResourceData.pSysMem = uploadData.data.data();
                    indexBufferResourceData.SysMemPitch = 0;
                    indexBufferResourceData.SysMemSlicePitch = 0;

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

                    std::copy(uploadData.data.begin(), uploadData.data.end(), static_cast<uint8_t*>(mappedSubResource.pData));

                    rendererD3D11->getContext()->Unmap(buffer, 0);
                }
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
