// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_DIRECT3D11

#include "BufferResourceD3D11.h"
#include "RendererD3D11.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        BufferResourceD3D11::BufferResourceD3D11()
        {
        }

        BufferResourceD3D11::~BufferResourceD3D11()
        {
            if (buffer)
            {
                buffer->Release();
            }
        }

        bool BufferResourceD3D11::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
                RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

                if (!data.empty())
                {
                    if (!buffer || data.size() > bufferSize)
                    {
                        if (buffer) buffer->Release();

                        bufferSize = static_cast<UINT>(data.size());

                        D3D11_BUFFER_DESC bufferDesc;
                        bufferDesc.ByteWidth = bufferSize;
                        bufferDesc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;

                        switch (usage)
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

                        bufferDesc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
                        bufferDesc.MiscFlags = 0;
                        bufferDesc.StructureByteStride = 0;

                        D3D11_SUBRESOURCE_DATA bufferResourceData;
                        bufferResourceData.pSysMem = data.data();
                        bufferResourceData.SysMemPitch = 0;
                        bufferResourceData.SysMemSlicePitch = 0;

                        HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&bufferDesc, &bufferResourceData, &buffer);
                        if (FAILED(hr))
                        {
                            Log(Log::Level::ERR) << "Failed to create Direct3D 11 buffer, error: " << hr;
                            return false;
                        }
                    }
                    else
                    {
                        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
                        mappedSubresource.pData = nullptr;
                        mappedSubresource.RowPitch = 0;
                        mappedSubresource.DepthPitch = 0;

                        HRESULT hr = rendererD3D11->getContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
                        if (FAILED(hr))
                        {
                            Log(Log::Level::ERR) << "Failed to lock Direct3D 11 buffer, error: " << hr;
                            return false;
                        }

                        std::copy(data.begin(), data.end(), static_cast<uint8_t*>(mappedSubresource.pData));

                        rendererD3D11->getContext()->Unmap(buffer, 0);
                    }
                }

                dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
