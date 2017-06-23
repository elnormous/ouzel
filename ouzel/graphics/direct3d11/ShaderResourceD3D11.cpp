// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_DIRECT3D11

#include "ShaderResourceD3D11.h"
#include "RendererD3D11.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        ShaderResourceD3D11::ShaderResourceD3D11()
        {
        }

        ShaderResourceD3D11::~ShaderResourceD3D11()
        {
            if (pixelShader)
            {
                pixelShader->Release();
            }

            if (vertexShader)
            {
                vertexShader->Release();
            }

            if (inputLayout)
            {
                inputLayout->Release();
            }

            if (pixelShaderConstantBuffer)
            {
                pixelShaderConstantBuffer->Release();
            }

            if (vertexShaderConstantBuffer)
            {
                vertexShaderConstantBuffer->Release();
            }
        }

        bool ShaderResourceD3D11::uploadBuffer(ID3D11Buffer* buffer, const void* data, uint32_t size)
        {
            RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

            D3D11_MAPPED_SUBRESOURCE mappedSubresource;
            HRESULT hr = rendererD3D11->getContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to lock Direct3D 11 buffer, error: " << hr;
                return false;
            }

            std::copy(static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + size, static_cast<uint8_t*>(mappedSubresource.pData));

            rendererD3D11->getContext()->Unmap(buffer, 0);

            return true;
        }

        static DXGI_FORMAT getVertexFormat(DataType dataType, bool normalized)
        {
            switch (dataType)
            {
                case DataType::BYTE:
                    return normalized ? DXGI_FORMAT_R8_SNORM : DXGI_FORMAT_R8_SINT;
                case DataType::BYTE_VECTOR2:
                    return normalized ? DXGI_FORMAT_R8G8_SNORM : DXGI_FORMAT_R8G8_SINT;
                case DataType::BYTE_VECTOR3:
                    return DXGI_FORMAT_UNKNOWN;
                case DataType::BYTE_VECTOR4:
                    return normalized ? DXGI_FORMAT_R8G8B8A8_SNORM : DXGI_FORMAT_R8G8B8A8_SINT;

                case DataType::UNSIGNED_BYTE:
                    return normalized ? DXGI_FORMAT_R8_UNORM : DXGI_FORMAT_R8_UINT;
                case DataType::UNSIGNED_BYTE_VECTOR2:
                    return normalized ? DXGI_FORMAT_R8G8_UNORM : DXGI_FORMAT_R8G8_UINT;
                case DataType::UNSIGNED_BYTE_VECTOR3:
                    return DXGI_FORMAT_UNKNOWN;
                case DataType::UNSIGNED_BYTE_VECTOR4:
                    return normalized ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UINT;

                case DataType::SHORT:
                    return normalized ? DXGI_FORMAT_R16_SNORM : DXGI_FORMAT_R16_SINT;
                case DataType::SHORT_VECTOR2:
                    return normalized ? DXGI_FORMAT_R16G16_SNORM : DXGI_FORMAT_R16G16_SINT;
                case DataType::SHORT_VECTOR3:
                    return DXGI_FORMAT_UNKNOWN;
                case DataType::SHORT_VECTOR4:
                    return normalized ? DXGI_FORMAT_R16G16B16A16_SNORM : DXGI_FORMAT_R16G16B16A16_SINT;

                case DataType::UNSIGNED_SHORT:
                    return normalized ? DXGI_FORMAT_R16_UNORM : DXGI_FORMAT_R16_UINT;
                case DataType::UNSIGNED_SHORT_VECTOR2:
                    return normalized ? DXGI_FORMAT_R16G16_UNORM : DXGI_FORMAT_R16G16_UINT;
                case DataType::UNSIGNED_SHORT_VECTOR3:
                    return DXGI_FORMAT_UNKNOWN;
                case DataType::UNSIGNED_SHORT_VECTOR4:
                    return normalized ? DXGI_FORMAT_R16G16B16A16_UNORM : DXGI_FORMAT_R16G16B16A16_UINT;

                case DataType::INTEGER:
                    return normalized ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32_SINT;
                case DataType::INTEGER_VECTOR2:
                    return normalized ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32G32_SINT;
                case DataType::INTEGER_VECTOR3:
                    return normalized ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32G32B32_SINT;
                case DataType::INTEGER_VECTOR4:
                    return normalized ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32G32B32A32_SINT;

                case DataType::UNSIGNED_INTEGER:
                    return normalized ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32_UINT;
                case DataType::UNSIGNED_INTEGER_VECTOR2:
                    return normalized ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32G32_UINT;
                case DataType::UNSIGNED_INTEGER_VECTOR3:
                    return normalized ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32G32B32_UINT;
                case DataType::UNSIGNED_INTEGER_VECTOR4:
                    return normalized ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32G32B32A32_UINT;

                case DataType::FLOAT:
                    return normalized ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32_FLOAT;
                case DataType::FLOAT_VECTOR2:
                    return normalized ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32G32_FLOAT;
                case DataType::FLOAT_VECTOR3:
                    return normalized ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32G32B32_FLOAT;
                case DataType::FLOAT_VECTOR4:
                    return normalized ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32G32B32A32_FLOAT;

                default:
                    return DXGI_FORMAT_UNKNOWN;
            }
        }

        bool ShaderResourceD3D11::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
                RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

                if (pixelShader)
                {
                    pixelShader->Release();
                    pixelShader = nullptr;
                }

                if (vertexShader)
                {
                    vertexShader->Release();
                    vertexShader = nullptr;
                }

                if (inputLayout)
                {
                    inputLayout->Release();
                    inputLayout = nullptr;
                }

                if (pixelShaderConstantBuffer)
                {
                    pixelShaderConstantBuffer->Release();
                    pixelShaderConstantBuffer = nullptr;
                }

                if (vertexShaderConstantBuffer)
                {
                    vertexShaderConstantBuffer->Release();
                    vertexShaderConstantBuffer = nullptr;
                }

                HRESULT hr = rendererD3D11->getDevice()->CreatePixelShader(pixelShaderData.data(), pixelShaderData.size(), NULL, &pixelShader);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create a Direct3D 11 pixel shader, error: " << hr;
                    return false;
                }
                
                hr = rendererD3D11->getDevice()->CreateVertexShader(vertexShaderData.data(), vertexShaderData.size(), NULL, &vertexShader);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create a Direct3D 11 vertex shader, error: " << hr;
                    return false;
                }

                std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputElements;

                UINT offset = 0;

                for (const VertexAttribute& vertexAttribute : vertexAttributes)
                {
                    DXGI_FORMAT vertexFormat = getVertexFormat(vertexAttribute.dataType, vertexAttribute.normalized);

                    if (vertexFormat == DXGI_FORMAT_UNKNOWN)
                    {
                        Log(Log::Level::ERR) << "Invalid vertex format";
                        return false;
                    }

                    const char* usage;

                    switch (vertexAttribute.usage)
                    {
                        case VertexAttribute::Usage::BINORMAL:
                            usage = "BINORMAL";
                            break;
                        case VertexAttribute::Usage::BLEND_INDICES:
                            usage = "BLENDINDICES";
                            break;
                        case VertexAttribute::Usage::BLEND_WEIGHT:
                            usage = "BLENDWEIGHT";
                            break;
                        case VertexAttribute::Usage::COLOR:
                            usage = "COLOR";
                            break;
                        case VertexAttribute::Usage::NORMAL:
                            usage = "NORMAL";
                            break;
                        case VertexAttribute::Usage::POSITION:
                            usage = "POSITION";
                            break;
                        case VertexAttribute::Usage::POSITION_TRANSFORMED:
                            usage = "POSITIONT";
                            break;
                        case VertexAttribute::Usage::POINT_SIZE:
                            usage = "PSIZE";
                            break;
                        case VertexAttribute::Usage::TANGENT:
                            usage = "TANGENT";
                            break;
                        case VertexAttribute::Usage::TEXTURE_COORDINATES:
                            usage = "TEXCOORD";
                            break;
                        default:
                            Log(Log::Level::ERR) << "Invalid vertex attribute usage";
                            return false;
                    }

                    vertexInputElements.push_back({
                        usage, vertexAttribute.index,
                        vertexFormat,
                        0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0
                    });
                    offset += getDataTypeSize(vertexAttribute.dataType);
                }

                hr = rendererD3D11->getDevice()->CreateInputLayout(
                    vertexInputElements.data(),
                    static_cast<UINT>(vertexInputElements.size()),
                    vertexShaderData.data(),
                    vertexShaderData.size(),
                    &inputLayout);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create Direct3D 11 input layout for vertex shader, error: " << hr;
                    return false;
                }

                if (!pixelShaderConstantInfo.empty())
                {
                    pixelShaderConstantLocations.clear();
                    pixelShaderConstantLocations.reserve(pixelShaderConstantInfo.size());

                    pixelShaderConstantSize = 0;

                    for (const Shader::ConstantInfo& info : pixelShaderConstantInfo)
                    {
                        pixelShaderConstantLocations.push_back({pixelShaderConstantSize, info.size});
                        pixelShaderConstantSize += info.size;
                    }
                }

                D3D11_BUFFER_DESC pixelShaderConstantBufferDesc;
                pixelShaderConstantBufferDesc.ByteWidth = static_cast<UINT>(pixelShaderConstantSize);
                pixelShaderConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
                pixelShaderConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                pixelShaderConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                pixelShaderConstantBufferDesc.MiscFlags = 0;
                pixelShaderConstantBufferDesc.StructureByteStride = 0;

                hr = rendererD3D11->getDevice()->CreateBuffer(&pixelShaderConstantBufferDesc, nullptr, &pixelShaderConstantBuffer);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create Direct3D 11 constant buffer, error: " << hr;
                    return false;
                }

                if (!vertexShaderConstantInfo.empty())
                {
                    vertexShaderConstantLocations.clear();
                    vertexShaderConstantLocations.reserve(vertexShaderConstantInfo.size());

                    vertexShaderConstantSize = 0;

                    for (const Shader::ConstantInfo& info : vertexShaderConstantInfo)
                    {
                        vertexShaderConstantLocations.push_back({vertexShaderConstantSize, info.size});
                        vertexShaderConstantSize += info.size;
                    }
                }

                D3D11_BUFFER_DESC vertexShaderConstantBufferDesc;
                vertexShaderConstantBufferDesc.ByteWidth = static_cast<UINT>(vertexShaderConstantSize);
                vertexShaderConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
                vertexShaderConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                vertexShaderConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                vertexShaderConstantBufferDesc.MiscFlags = 0;
                vertexShaderConstantBufferDesc.StructureByteStride = 0;

                hr = rendererD3D11->getDevice()->CreateBuffer(&vertexShaderConstantBufferDesc, nullptr, &vertexShaderConstantBuffer);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create Direct3D 11 constant buffer, error: " << hr;
                    return false;
                }

                dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
