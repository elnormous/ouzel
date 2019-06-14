// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "D3D11Buffer.hpp"
#include "D3D11RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
        {
            Buffer::Buffer(RenderDevice& renderDevice,
                           BufferType initType,
                           uint32_t initFlags,
                           const std::vector<uint8_t>& data,
                           uint32_t initSize):
                RenderResource(renderDevice),
                type(initType),
                flags(initFlags),
                size(static_cast<UINT>(initSize))
            {
                createBuffer(initSize, data);
            }

            Buffer::~Buffer()
            {
                if (buffer) buffer->Release();
            }

            void Buffer::setData(const std::vector<uint8_t>& data)
            {
                if (!(flags & Flags::DYNAMIC))
                    throw std::runtime_error("Buffer is not dynamic");

                if (data.empty())
                    throw std::runtime_error("Data is empty");

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

                        HRESULT hr;
                        if (FAILED(hr = renderDevice.getContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
                            throw std::system_error(hr, errorCategory, "Failed to lock Direct3D 11 buffer");

                        std::copy(data.begin(), data.end(), static_cast<uint8_t*>(mappedSubresource.pData));

                        renderDevice.getContext()->Unmap(buffer, 0);
                    }
                }
            }

            void Buffer::createBuffer(UINT newSize, const std::vector<uint8_t>& data)
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
                    bufferDesc.Usage = (flags & Flags::DYNAMIC) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;

                    switch (usage)
                    {
                        case graphics::Buffer::Usage::INDEX:
                            bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                            break;
                        case graphics::Buffer::Usage::VERTEX:
                            bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                            break;
                        default:
                            throw std::runtime_error("Unsupported buffer type");
                    }

                    bufferDesc.CPUAccessFlags = (flags & Flags::DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;
                    bufferDesc.MiscFlags = 0;
                    bufferDesc.StructureByteStride = 0;

                    HRESULT hr;

                    if (data.empty())
                    {
                        if (FAILED(hr = renderDevice.getDevice()->CreateBuffer(&bufferDesc, nullptr, &buffer)))
                            throw std::system_error(hr, errorCategory, "Failed to create Direct3D 11 buffer");
                    }
                    else
                    {
                        D3D11_SUBRESOURCE_DATA bufferResourceData;
                        bufferResourceData.pSysMem = data.data();
                        bufferResourceData.SysMemPitch = 0;
                        bufferResourceData.SysMemSlicePitch = 0;

                        if (FAILED(hr = renderDevice.getDevice()->CreateBuffer(&bufferDesc, &bufferResourceData, &buffer)))
                            throw std::system_error(hr, errorCategory, "Failed to create Direct3D 11 buffer");
                    }
                }
            }
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif
