// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BufferD3D11.h"
#include "RendererD3D11.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        BufferD3D11::BufferD3D11()
        {
        }

        BufferD3D11::~BufferD3D11()
        {
            if (buffer)
            {
                buffer->Release();
            }
        }

        void BufferD3D11::free()
        {
            BufferResource::free();

            if (buffer)
            {
                buffer->Release();
                buffer = nullptr;
            }

            bufferSize = 0;
        }

        bool BufferD3D11::upload()
        {
            if (!BufferResource::upload())
            {
                return false;
            }

            RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

            if (!uploadData.data.empty())
            {
                if (!buffer || uploadData.data.size() > bufferSize)
                {
                    if (buffer) buffer->Release();

                    bufferSize = static_cast<UINT>(uploadData.data.size());

                    D3D11_BUFFER_DESC bufferDesc;
                    bufferDesc.ByteWidth = bufferSize;
                    bufferDesc.Usage = uploadData.dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;

                    switch (uploadData.usage)
                    {
                        case Buffer::Usage::INDEX:
                            bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                            break;
                        case Buffer::Usage::VERTEX:
                            bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                            break;
                        default:
                            Log(Log::Level::ERR) << "Unsupported buffer type";
                            return false;
                    }
                    
                    bufferDesc.CPUAccessFlags = uploadData.dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
                    bufferDesc.MiscFlags = 0;
                    bufferDesc.StructureByteStride = 0;

                    D3D11_SUBRESOURCE_DATA bufferResourceData;
                    bufferResourceData.pSysMem = uploadData.data.data();
                    bufferResourceData.SysMemPitch = 0;
                    bufferResourceData.SysMemSlicePitch = 0;

                    HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&bufferDesc, &bufferResourceData, &buffer);
                    if (FAILED(hr))
                    {
                        Log(Log::Level::ERR) << "Failed to create Direct3D 11 buffer";
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
