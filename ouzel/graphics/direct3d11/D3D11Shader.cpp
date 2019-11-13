// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <stdexcept>
#include "D3D11Shader.hpp"
#include "D3D11RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
        {
            namespace
            {
                constexpr DXGI_FORMAT getVertexFormat(DataType dataType)
                {
                    switch (dataType)
                    {
                        case DataType::Byte: return DXGI_FORMAT_R8_SINT;
                        case DataType::ByteNorm: return DXGI_FORMAT_R8_SNORM;
                        case DataType::UnsignedByte: return DXGI_FORMAT_R8_UINT;
                        case DataType::UnsignedByteNorm: return DXGI_FORMAT_R8_UNORM;

                        case DataType::ByteVector2: return DXGI_FORMAT_R8G8_SINT;
                        case DataType::ByteVector2Norm: return DXGI_FORMAT_R8G8_SNORM;
                        case DataType::UnsignedByteVector2: return DXGI_FORMAT_R8G8_UINT;
                        case DataType::UnsignedByteVector2Norm: return DXGI_FORMAT_R8G8_UNORM;

                        case DataType::ByteVector3: return DXGI_FORMAT_UNKNOWN;
                        case DataType::ByteVector3Norm: return DXGI_FORMAT_UNKNOWN;
                        case DataType::UnsignedByteVector3: return DXGI_FORMAT_UNKNOWN;
                        case DataType::UnsignedByteVector3Norm: return DXGI_FORMAT_UNKNOWN;

                        case DataType::ByteVector4: return DXGI_FORMAT_R8G8B8A8_SINT;
                        case DataType::ByteVector4Norm: return DXGI_FORMAT_R8G8B8A8_SNORM;
                        case DataType::UnsignedByteVector4: return DXGI_FORMAT_R8G8B8A8_UINT;
                        case DataType::UnsignedByteVector4Norm: return DXGI_FORMAT_R8G8B8A8_UNORM;

                        case DataType::Short: return DXGI_FORMAT_R16_SINT;
                        case DataType::ShortNorm: return DXGI_FORMAT_R16_SNORM;
                        case DataType::UnsignedShort: return DXGI_FORMAT_R16_UINT;
                        case DataType::UnsignedShortNorm: return DXGI_FORMAT_R16_UNORM;

                        case DataType::ShortVector2: return DXGI_FORMAT_R16G16_SINT;
                        case DataType::ShortVector2Norm: return DXGI_FORMAT_R16G16_SNORM;
                        case DataType::UnsignedShortVector2: return DXGI_FORMAT_R16G16_UINT;
                        case DataType::UnsignedShortVector2Norm: return DXGI_FORMAT_R16G16_UNORM;

                        case DataType::ShortVector3: return DXGI_FORMAT_UNKNOWN;
                        case DataType::ShortVector3Norm: return DXGI_FORMAT_UNKNOWN;
                        case DataType::UnsignedShortVector3: return DXGI_FORMAT_UNKNOWN;
                        case DataType::UnsignedShortVector3Norm: return DXGI_FORMAT_UNKNOWN;

                        case DataType::ShortVector4: return DXGI_FORMAT_R16G16B16A16_SINT;
                        case DataType::ShortVector4Norm: return DXGI_FORMAT_R16G16B16A16_SNORM;
                        case DataType::UnsignedShortVector4: return DXGI_FORMAT_R16G16B16A16_UINT;
                        case DataType::UnsignedShortVector4Norm: return DXGI_FORMAT_R16G16B16A16_UNORM;

                        case DataType::Integer: return DXGI_FORMAT_R32_SINT;
                        case DataType::UnsignedInteger: return DXGI_FORMAT_R32_UINT;

                        case DataType::IntegerVector2: return DXGI_FORMAT_R32G32_SINT;
                        case DataType::UnsignedIntegerVector2: return DXGI_FORMAT_R32G32_UINT;

                        case DataType::IntegerVector3: return DXGI_FORMAT_R32G32B32_SINT;
                        case DataType::UnsignedIntegerVector3: return DXGI_FORMAT_R32G32B32_UINT;

                        case DataType::IntegerVector4: return DXGI_FORMAT_R32G32B32A32_SINT;
                        case DataType::UnsignedIntegerVector4: return DXGI_FORMAT_R32G32B32A32_UINT;

                        case DataType::Float: return DXGI_FORMAT_R32_FLOAT;
                        case DataType::FloatVector2: return DXGI_FORMAT_R32G32_FLOAT;
                        case DataType::FloatVector3: return DXGI_FORMAT_R32G32B32_FLOAT;
                        case DataType::FloatVector4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
                        case DataType::FloatMatrix3: return DXGI_FORMAT_UNKNOWN;
                        case DataType::FloatMatrix4: return DXGI_FORMAT_UNKNOWN;

                        default: throw std::runtime_error("Invalid data type");
                    }
                }
            }

            Shader::Shader(RenderDevice& initRenderDevice,
                           const std::vector<uint8_t>& fragmentShaderData,
                           const std::vector<uint8_t>& vertexShaderData,
                           const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                           const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                           const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                           uint32_t,
                           uint32_t,
                           const std::string&,
                           const std::string&):
                RenderResource(initRenderDevice),
                vertexAttributes(initVertexAttributes),
                fragmentShaderConstantInfo(initFragmentShaderConstantInfo),
                vertexShaderConstantInfo(initVertexShaderConstantInfo)
            {
				ID3D11PixelShader* newPixelShader;

                HRESULT hr;
                if (FAILED(hr = renderDevice.getDevice()->CreatePixelShader(fragmentShaderData.data(), fragmentShaderData.size(), nullptr, &newPixelShader)))
                    throw std::system_error(hr, getErrorCategory(), "Failed to create a Direct3D 11 pixel shader");

				fragmentShader = newPixelShader;

				ID3D11VertexShader* newVertexShader;

                if (FAILED(hr = renderDevice.getDevice()->CreateVertexShader(vertexShaderData.data(), vertexShaderData.size(), nullptr, &newVertexShader)))
                    throw std::system_error(hr, getErrorCategory(), "Failed to create a Direct3D 11 vertex shader");

				vertexShader = newVertexShader;

                std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputElements;

                UINT offset = 0;

                for (const Vertex::Attribute& vertexAttribute : RenderDevice::VERTEX_ATTRIBUTES)
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
                            case Vertex::Attribute::Usage::Binormal:
                                semantic = "BINORMAL";
                                break;
                            case Vertex::Attribute::Usage::BlendIndices:
                                semantic = "BLENDINDICES";
                                break;
                            case Vertex::Attribute::Usage::BlendWeight:
                                semantic = "BLENDWEIGHT";
                                break;
                            case Vertex::Attribute::Usage::Color:
                                semantic = "COLOR";
                                break;
                            case Vertex::Attribute::Usage::Normal:
                                semantic = "NORMAL";
                                break;
                            case Vertex::Attribute::Usage::Position:
                                semantic = "POSITION";
                                break;
                            case Vertex::Attribute::Usage::PositionTransformed:
                                semantic = "POSITIONT";
                                break;
                            case Vertex::Attribute::Usage::PointSize:
                                semantic = "PSIZE";
                                break;
                            case Vertex::Attribute::Usage::Tangent:
                                semantic = "TANGENT";
                                break;
                            case Vertex::Attribute::Usage::TextureCoordinates0:
                                semantic = "TEXCOORD";
                                break;
                            case Vertex::Attribute::Usage::TextureCoordinates1:
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

				ID3D11InputLayout* newInputLayout;

                if (FAILED(hr = renderDevice.getDevice()->CreateInputLayout(vertexInputElements.data(),
                                                                            static_cast<UINT>(vertexInputElements.size()),
                                                                            vertexShaderData.data(),
                                                                            vertexShaderData.size(),
                                                                            &newInputLayout)))
                    throw std::system_error(hr, getErrorCategory(), "Failed to create Direct3D 11 input layout for vertex shader");

				inputLayout = newInputLayout;

                if (!fragmentShaderConstantInfo.empty())
                {
                    fragmentShaderConstantLocations.reserve(fragmentShaderConstantInfo.size());

                    for (const std::pair<std::string, DataType>& info : fragmentShaderConstantInfo)
                    {
                        const uint32_t size = getDataTypeSize(info.second);
                        fragmentShaderConstantLocations.emplace_back(fragmentShaderConstantSize, size);
                        fragmentShaderConstantSize += size;
                    }
                }

                D3D11_BUFFER_DESC fragmentShaderConstantBufferDesc;
                fragmentShaderConstantBufferDesc.ByteWidth = static_cast<UINT>(fragmentShaderConstantSize);
                fragmentShaderConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
                fragmentShaderConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                fragmentShaderConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                fragmentShaderConstantBufferDesc.MiscFlags = 0;
                fragmentShaderConstantBufferDesc.StructureByteStride = 0;

				ID3D11Buffer* newFragmentShaderConstantBuffer;

                if (FAILED(hr = renderDevice.getDevice()->CreateBuffer(&fragmentShaderConstantBufferDesc, nullptr,
					                                                   &newFragmentShaderConstantBuffer)))
                    throw std::system_error(hr, getErrorCategory(), "Failed to create Direct3D 11 constant buffer");

				fragmentShaderConstantBuffer = newFragmentShaderConstantBuffer;

                if (!vertexShaderConstantInfo.empty())
                {
                    vertexShaderConstantLocations.reserve(vertexShaderConstantInfo.size());

                    for (const std::pair<std::string, DataType>& info : vertexShaderConstantInfo)
                    {
                        const uint32_t size = getDataTypeSize(info.second);
                        vertexShaderConstantLocations.emplace_back(vertexShaderConstantSize, size);
                        vertexShaderConstantSize += size;
                    }
                }

                D3D11_BUFFER_DESC vertexShaderConstantBufferDesc;
                vertexShaderConstantBufferDesc.ByteWidth = static_cast<UINT>(vertexShaderConstantSize);
                vertexShaderConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
                vertexShaderConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                vertexShaderConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                vertexShaderConstantBufferDesc.MiscFlags = 0;
                vertexShaderConstantBufferDesc.StructureByteStride = 0;

				ID3D11Buffer* newVertexShaderConstantBuffer;

                if (FAILED(hr = renderDevice.getDevice()->CreateBuffer(&vertexShaderConstantBufferDesc, nullptr,
					                                                   &newVertexShaderConstantBuffer)))
                    throw std::system_error(hr, getErrorCategory(), "Failed to create Direct3D 11 constant buffer");

				vertexShaderConstantBuffer = newVertexShaderConstantBuffer;
            }
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif
