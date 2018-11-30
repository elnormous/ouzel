// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "ShaderD3D11.hpp"
#include "RenderDeviceD3D11.hpp"

namespace ouzel
{
    namespace graphics
    {
        static DXGI_FORMAT getVertexFormat(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::BYTE: return DXGI_FORMAT_R8_SINT;
                case DataType::BYTE_NORM: return DXGI_FORMAT_R8_SNORM;
                case DataType::UNSIGNED_BYTE: return DXGI_FORMAT_R8_UINT;
                case DataType::UNSIGNED_BYTE_NORM: return DXGI_FORMAT_R8_UNORM;

                case DataType::BYTE_VECTOR2: return DXGI_FORMAT_R8G8_SINT;
                case DataType::BYTE_VECTOR2_NORM: return DXGI_FORMAT_R8G8_SNORM;
                case DataType::UNSIGNED_BYTE_VECTOR2: return DXGI_FORMAT_R8G8_UINT;
                case DataType::UNSIGNED_BYTE_VECTOR2_NORM: return DXGI_FORMAT_R8G8_UNORM;

                case DataType::BYTE_VECTOR3: return DXGI_FORMAT_UNKNOWN;
                case DataType::BYTE_VECTOR3_NORM: return DXGI_FORMAT_UNKNOWN;
                case DataType::UNSIGNED_BYTE_VECTOR3: return DXGI_FORMAT_UNKNOWN;
                case DataType::UNSIGNED_BYTE_VECTOR3_NORM: return DXGI_FORMAT_UNKNOWN;

                case DataType::BYTE_VECTOR4: return DXGI_FORMAT_R8G8B8A8_SINT;
                case DataType::BYTE_VECTOR4_NORM: return DXGI_FORMAT_R8G8B8A8_SNORM;
                case DataType::UNSIGNED_BYTE_VECTOR4: return DXGI_FORMAT_R8G8B8A8_UINT;
                case DataType::UNSIGNED_BYTE_VECTOR4_NORM: return DXGI_FORMAT_R8G8B8A8_UNORM;

                case DataType::SHORT: return DXGI_FORMAT_R16_SINT;
                case DataType::SHORT_NORM: return DXGI_FORMAT_R16_SNORM;
                case DataType::UNSIGNED_SHORT: return DXGI_FORMAT_R16_UINT;
                case DataType::UNSIGNED_SHORT_NORM: return DXGI_FORMAT_R16_UNORM;

                case DataType::SHORT_VECTOR2: return DXGI_FORMAT_R16G16_SINT;
                case DataType::SHORT_VECTOR2_NORM: return DXGI_FORMAT_R16G16_SNORM;
                case DataType::UNSIGNED_SHORT_VECTOR2: return DXGI_FORMAT_R16G16_UINT;
                case DataType::UNSIGNED_SHORT_VECTOR2_NORM: return DXGI_FORMAT_R16G16_UNORM;

                case DataType::SHORT_VECTOR3: return DXGI_FORMAT_UNKNOWN;
                case DataType::SHORT_VECTOR3_NORM: return DXGI_FORMAT_UNKNOWN;
                case DataType::UNSIGNED_SHORT_VECTOR3: return DXGI_FORMAT_UNKNOWN;
                case DataType::UNSIGNED_SHORT_VECTOR3_NORM: return DXGI_FORMAT_UNKNOWN;

                case DataType::SHORT_VECTOR4: return DXGI_FORMAT_R16G16B16A16_SINT;
                case DataType::SHORT_VECTOR4_NORM: return DXGI_FORMAT_R16G16B16A16_SNORM;
                case DataType::UNSIGNED_SHORT_VECTOR4: return DXGI_FORMAT_R16G16B16A16_UINT;
                case DataType::UNSIGNED_SHORT_VECTOR4_NORM: return DXGI_FORMAT_R16G16B16A16_UNORM;

                case DataType::INTEGER: return DXGI_FORMAT_R32_SINT;
                case DataType::UNSIGNED_INTEGER: return DXGI_FORMAT_R32_UINT;

                case DataType::INTEGER_VECTOR2: return DXGI_FORMAT_R32G32_SINT;
                case DataType::UNSIGNED_INTEGER_VECTOR2: return DXGI_FORMAT_R32G32_UINT;

                case DataType::INTEGER_VECTOR3: return DXGI_FORMAT_R32G32B32_SINT;
                case DataType::UNSIGNED_INTEGER_VECTOR3: return DXGI_FORMAT_R32G32B32_UINT;

                case DataType::INTEGER_VECTOR4: return DXGI_FORMAT_R32G32B32A32_SINT;
                case DataType::UNSIGNED_INTEGER_VECTOR4: return DXGI_FORMAT_R32G32B32A32_UINT;

                case DataType::FLOAT: return DXGI_FORMAT_R32_FLOAT;
                case DataType::FLOAT_VECTOR2: return DXGI_FORMAT_R32G32_FLOAT;
                case DataType::FLOAT_VECTOR3: return DXGI_FORMAT_R32G32B32_FLOAT;
                case DataType::FLOAT_VECTOR4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
                case DataType::FLOAT_MATRIX3: return DXGI_FORMAT_UNKNOWN;
                case DataType::FLOAT_MATRIX4: return DXGI_FORMAT_UNKNOWN;

                case DataType::NONE: return DXGI_FORMAT_UNKNOWN;
            }

            return DXGI_FORMAT_UNKNOWN;
        }

        ShaderD3D11::ShaderD3D11(RenderDeviceD3D11& renderDeviceD3D11,
                                 const std::vector<uint8_t>& fragmentShaderData,
                                 const std::vector<uint8_t>& vertexShaderData,
                                 const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                                 const std::vector<Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                                 const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                                 uint32_t,
                                 uint32_t,
                                 const std::string&,
                                 const std::string&):
            RenderResourceD3D11(renderDeviceD3D11),
            vertexAttributes(newVertexAttributes),
            fragmentShaderConstantInfo(newFragmentShaderConstantInfo),
            vertexShaderConstantInfo(newVertexShaderConstantInfo)
        {
            HRESULT hr;
            if (FAILED(hr = renderDeviceD3D11.getDevice()->CreatePixelShader(fragmentShaderData.data(), fragmentShaderData.size(), nullptr, &fragmentShader)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create a Direct3D 11 pixel shader");

            if (vertexShader) vertexShader->Release();

            if (FAILED(hr = renderDeviceD3D11.getDevice()->CreateVertexShader(vertexShaderData.data(), vertexShaderData.size(), nullptr, &vertexShader)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create a Direct3D 11 vertex shader");

            std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputElements;

            UINT offset = 0;

            for (const Vertex::Attribute& vertexAttribute : Vertex::ATTRIBUTES)
            {
                if (vertexAttributes.find(vertexAttribute.usage) != vertexAttributes.end())
                {
                    DXGI_FORMAT vertexFormat = getVertexFormat(vertexAttribute.dataType);

                    if (vertexFormat == DXGI_FORMAT_UNKNOWN)
                        throw std::runtime_error("Invalid vertex format");

                    const char* semantic;
                    UINT index = 0;

                    switch (vertexAttribute.usage)
                    {
                        case Vertex::Attribute::Usage::BINORMAL:
                            semantic = "BINORMAL";
                            break;
                        case Vertex::Attribute::Usage::BLEND_INDICES:
                            semantic = "BLENDINDICES";
                            break;
                        case Vertex::Attribute::Usage::BLEND_WEIGHT:
                            semantic = "BLENDWEIGHT";
                            break;
                        case Vertex::Attribute::Usage::COLOR:
                            semantic = "COLOR";
                            break;
                        case Vertex::Attribute::Usage::NORMAL:
                            semantic = "NORMAL";
                            break;
                        case Vertex::Attribute::Usage::POSITION:
                            semantic = "POSITION";
                            break;
                        case Vertex::Attribute::Usage::POSITION_TRANSFORMED:
                            semantic = "POSITIONT";
                            break;
                        case Vertex::Attribute::Usage::POINT_SIZE:
                            semantic = "PSIZE";
                            break;
                        case Vertex::Attribute::Usage::TANGENT:
                            semantic = "TANGENT";
                            break;
                        case Vertex::Attribute::Usage::TEXTURE_COORDINATES0:
                            semantic = "TEXCOORD";
                            break;
                        case Vertex::Attribute::Usage::TEXTURE_COORDINATES1:
                            semantic = "TEXCOORD";
                            index = 1;
                            break;
                        default:
                            throw std::runtime_error("Invalid vertex attribute usage");
                    }

                    vertexInputElements.push_back({
                        semantic, index,
                        vertexFormat,
                        0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0
                    });
                }

                offset += getDataTypeSize(vertexAttribute.dataType);
            }

            if (inputLayout) inputLayout->Release();

            if (FAILED(hr = renderDeviceD3D11.getDevice()->CreateInputLayout(vertexInputElements.data(),
                                                                             static_cast<UINT>(vertexInputElements.size()),
                                                                             vertexShaderData.data(),
                                                                             vertexShaderData.size(),
                                                                             &inputLayout)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 input layout for vertex shader");

            if (!fragmentShaderConstantInfo.empty())
            {
                fragmentShaderConstantLocations.clear();
                fragmentShaderConstantLocations.reserve(fragmentShaderConstantInfo.size());

                fragmentShaderConstantSize = 0;

                for (const Shader::ConstantInfo& info : fragmentShaderConstantInfo)
                {
                    fragmentShaderConstantLocations.push_back({fragmentShaderConstantSize, info.size});
                    fragmentShaderConstantSize += info.size;
                }
            }

            D3D11_BUFFER_DESC fragmentShaderConstantBufferDesc;
            fragmentShaderConstantBufferDesc.ByteWidth = static_cast<UINT>(fragmentShaderConstantSize);
            fragmentShaderConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            fragmentShaderConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            fragmentShaderConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            fragmentShaderConstantBufferDesc.MiscFlags = 0;
            fragmentShaderConstantBufferDesc.StructureByteStride = 0;

            if (fragmentShaderConstantBuffer) fragmentShaderConstantBuffer->Release();

            if (FAILED(hr = renderDeviceD3D11.getDevice()->CreateBuffer(&fragmentShaderConstantBufferDesc, nullptr, &fragmentShaderConstantBuffer)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 constant buffer");

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

            if (vertexShaderConstantBuffer) vertexShaderConstantBuffer->Release();

            if (FAILED(hr = renderDeviceD3D11.getDevice()->CreateBuffer(&vertexShaderConstantBufferDesc, nullptr, &vertexShaderConstantBuffer)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 constant buffer");
        }

        ShaderD3D11::~ShaderD3D11()
        {
            if (fragmentShader)
                fragmentShader->Release();

            if (vertexShader)
                vertexShader->Release();

            if (inputLayout)
                inputLayout->Release();

            if (fragmentShaderConstantBuffer)
                fragmentShaderConstantBuffer->Release();

            if (vertexShaderConstantBuffer)
                vertexShaderConstantBuffer->Release();
        }
    } // namespace graphics
} // namespace ouzel

#endif
