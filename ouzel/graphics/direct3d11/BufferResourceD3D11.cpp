// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "BufferResourceD3D11.hpp"
#include "RenderDeviceD3D11.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace graphics
    {
        BufferResourceD3D11::BufferResourceD3D11(RenderDeviceD3D11& renderDeviceD3D11,
                                                 Buffer::Usage newUsage, uint32_t newFlags,
                                                 const std::vector<uint8_t>& data,
                                                 uint32_t newSize):
            RenderResourceD3D11(renderDeviceD3D11),
            usage(newUsage),
            flags(newFlags),
            size(static_cast<UINT>(newSize))
        {
            createBuffer(newSize, data);
        }

        BufferResourceD3D11::~BufferResourceD3D11()
        {
            if (buffer) buffer->Release();
        }

        void BufferResourceD3D11::setData(const std::vector<uint8_t>& data)
        {
            if (!(flags & Buffer::DYNAMIC))
                throw DataError("Buffer is not dynamic");

            if (data.empty())
                throw DataError("Data is empty");

            if (!buffer || data.size() > size)
                createBuffer(static_cast<UINT>(data.size()), data);
            else
            {
                if (!data.empty())
                {
                    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
                    mappedSubresource.pData = nullptr;
                    mappedSubresource.RowPitch = 0;
                    mappedSubresource.DepthPitch = 0;

                    HRESULT hr = renderDevice.getContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
                    if (FAILED(hr))
                        throw DataError("Failed to lock Direct3D 11 buffer, error: " + std::to_string(hr));

                    std::copy(data.begin(), data.end(), static_cast<uint8_t*>(mappedSubresource.pData));

                    renderDevice.getContext()->Unmap(buffer, 0);
                }
            }
        }

        void BufferResourceD3D11::createBuffer(UINT newSize, const std::vector<uint8_t>& data)
        {
            if (buffer)
            {
                buffer->Release();
                buffer = nullptr;
            }

            if (newSize)
            {
                size = newSize;

                D3D11_BUFFER_DESC bufferDesc;
                bufferDesc.ByteWidth = size;
                bufferDesc.Usage = (flags & Texture::DYNAMIC) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;

                switch (usage)
                {
                    case Buffer::Usage::INDEX:
                        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                        break;
                    case Buffer::Usage::VERTEX:
                        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                        break;
                    default:
                        throw DataError("Unsupported buffer type");
                }

                bufferDesc.CPUAccessFlags = (flags & Texture::DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;
                bufferDesc.MiscFlags = 0;
                bufferDesc.StructureByteStride = 0;

                HRESULT hr;

                if (data.empty())
                    hr = renderDevice.getDevice()->CreateBuffer(&bufferDesc, nullptr, &buffer);
                else
                {
                    D3D11_SUBRESOURCE_DATA bufferResourceData;
                    bufferResourceData.pSysMem = data.data();
                    bufferResourceData.SysMemPitch = 0;
                    bufferResourceData.SysMemSlicePitch = 0;

                    hr = renderDevice.getDevice()->CreateBuffer(&bufferDesc, &bufferResourceData, &buffer);
                }

                if (FAILED(hr))
                    throw DataError("Failed to create Direct3D 11 buffer, error: " + std::to_string(hr));
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
