// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <cstring>
#include "D3D11Buffer.hpp"
#include "D3D11ErrorCategory.hpp"
#include "D3D11MappedSubresource.hpp"
#include "D3D11RenderDevice.hpp"

namespace ouzel::graphics::d3d11
{
    Buffer::Buffer(RenderDevice& initRenderDevice,
                   BufferType initType,
                   Flags initFlags,
                   const std::vector<std::uint8_t>& data,
                   std::uint32_t initSize):
        RenderResource{initRenderDevice},
        type{initType},
        flags{initFlags},
        size{static_cast<UINT>(initSize)}
    {
        createBuffer(initSize, data);
    }

    void Buffer::setData(const std::vector<std::uint8_t>& data)
    {
        if ((flags & Flags::dynamic) != Flags::dynamic)
            throw std::runtime_error{"Buffer is not dynamic"};

        if (data.empty())
            throw std::runtime_error{"Data is empty"};

        if (!buffer || data.size() > size)
            createBuffer(static_cast<UINT>(data.size()), data);
        else
        {
            if (!data.empty())
            {
                MappedSubresource mapped{renderDevice.getContext().get()};
                const auto mappedSubresource = mapped.map(buffer.get(), 0, D3D11_MAP_WRITE_DISCARD);

                std::memcpy(mappedSubresource.pData, data.data(), data.size());
            }
        }
    }

    void Buffer::createBuffer(UINT newSize, const std::vector<std::uint8_t>& data)
    {
        if (newSize)
        {
            size = newSize;

            D3D11_BUFFER_DESC bufferDesc;
            bufferDesc.ByteWidth = size;
            bufferDesc.Usage = (flags & Flags::dynamic) == Flags::dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;

            switch (type)
            {
                case BufferType::index:
                    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                    break;
                case BufferType::vertex:
                    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                    break;
                default:
                    throw std::runtime_error{"Unsupported buffer type"};
            }

            bufferDesc.CPUAccessFlags = (flags & Flags::dynamic) == Flags::dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
            bufferDesc.MiscFlags = 0;
            bufferDesc.StructureByteStride = 0;

            if (data.empty())
            {
                ID3D11Buffer* newBuffer;
                if (const auto hr = renderDevice.getDevice()->CreateBuffer(&bufferDesc, nullptr, &newBuffer); FAILED(hr))
                    throw std::system_error{hr, errorCategory, "Failed to create Direct3D 11 buffer"};

                buffer = newBuffer;
            }
            else
            {
                D3D11_SUBRESOURCE_DATA bufferResourceData;
                bufferResourceData.pSysMem = data.data();
                bufferResourceData.SysMemPitch = 0;
                bufferResourceData.SysMemSlicePitch = 0;

                ID3D11Buffer* newBuffer;
                if (const auto hr = renderDevice.getDevice()->CreateBuffer(&bufferDesc, &bufferResourceData, &newBuffer); FAILED(hr))
                    throw std::system_error{hr, errorCategory, "Failed to create Direct3D 11 buffer"};

                buffer = newBuffer;
            }
        }
    }
}

#endif
