// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <stdexcept>
#include "D3D11Shader.hpp"
#include "D3D11RenderDevice.hpp"

namespace ouzel::graphics::d3d11
{
    namespace
    {
        constexpr DXGI_FORMAT getVertexFormat(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::byte: return DXGI_FORMAT_R8_SINT;
                case DataType::byteNorm: return DXGI_FORMAT_R8_SNORM;
                case DataType::unsignedByte: return DXGI_FORMAT_R8_UINT;
                case DataType::unsignedByteNorm: return DXGI_FORMAT_R8_UNORM;

                case DataType::byteVector2: return DXGI_FORMAT_R8G8_SINT;
                case DataType::byteVector2Norm: return DXGI_FORMAT_R8G8_SNORM;
                case DataType::unsignedByteVector2: return DXGI_FORMAT_R8G8_UINT;
                case DataType::unsignedByteVector2Norm: return DXGI_FORMAT_R8G8_UNORM;

                case DataType::byteVector3: return DXGI_FORMAT_UNKNOWN;
                case DataType::byteVector3Norm: return DXGI_FORMAT_UNKNOWN;
                case DataType::unsignedByteVector3: return DXGI_FORMAT_UNKNOWN;
                case DataType::unsignedByteVector3Norm: return DXGI_FORMAT_UNKNOWN;

                case DataType::byteVector4: return DXGI_FORMAT_R8G8B8A8_SINT;
                case DataType::byteVector4Norm: return DXGI_FORMAT_R8G8B8A8_SNORM;
                case DataType::unsignedByteVector4: return DXGI_FORMAT_R8G8B8A8_UINT;
                case DataType::unsignedByteVector4Norm: return DXGI_FORMAT_R8G8B8A8_UNORM;

                case DataType::integer16: return DXGI_FORMAT_R16_SINT;
                case DataType::integer16Norm: return DXGI_FORMAT_R16_SNORM;
                case DataType::unsignedInteger16: return DXGI_FORMAT_R16_UINT;
                case DataType::unsignedInteger16Norm: return DXGI_FORMAT_R16_UNORM;

                case DataType::integer16Vector2: return DXGI_FORMAT_R16G16_SINT;
                case DataType::integer16Vector2Norm: return DXGI_FORMAT_R16G16_SNORM;
                case DataType::unsignedInteger16Vector2: return DXGI_FORMAT_R16G16_UINT;
                case DataType::unsignedInteger16Vector2Norm: return DXGI_FORMAT_R16G16_UNORM;

                case DataType::integer16Vector3: return DXGI_FORMAT_UNKNOWN;
                case DataType::integer16Vector3Norm: return DXGI_FORMAT_UNKNOWN;
                case DataType::unsignedInteger16Vector3: return DXGI_FORMAT_UNKNOWN;
                case DataType::unsignedInteger16Vector3Norm: return DXGI_FORMAT_UNKNOWN;

                case DataType::integer16Vector4: return DXGI_FORMAT_R16G16B16A16_SINT;
                case DataType::integer16Vector4Norm: return DXGI_FORMAT_R16G16B16A16_SNORM;
                case DataType::unsignedInteger16Vector4: return DXGI_FORMAT_R16G16B16A16_UINT;
                case DataType::unsignedInteger16Vector4Norm: return DXGI_FORMAT_R16G16B16A16_UNORM;

                case DataType::integer32: return DXGI_FORMAT_R32_SINT;
                case DataType::unsignedInteger32: return DXGI_FORMAT_R32_UINT;

                case DataType::integer32Vector2: return DXGI_FORMAT_R32G32_SINT;
                case DataType::unsignedInteger32Vector2: return DXGI_FORMAT_R32G32_UINT;

                case DataType::integer32Vector3: return DXGI_FORMAT_R32G32B32_SINT;
                case DataType::unsignedInteger32Vector3: return DXGI_FORMAT_R32G32B32_UINT;

                case DataType::integer32Vector4: return DXGI_FORMAT_R32G32B32A32_SINT;
                case DataType::unsignedInteger32Vector4: return DXGI_FORMAT_R32G32B32A32_UINT;

                case DataType::float32: return DXGI_FORMAT_R32_FLOAT;
                case DataType::float32Vector2: return DXGI_FORMAT_R32G32_FLOAT;
                case DataType::float32Vector3: return DXGI_FORMAT_R32G32B32_FLOAT;
                case DataType::float32Vector4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
                case DataType::float32Matrix3: return DXGI_FORMAT_UNKNOWN;
                case DataType::float32Matrix4: return DXGI_FORMAT_UNKNOWN;

                default: throw std::runtime_error("Invalid data type");
            }
        }
    }

    Shader::Shader(RenderDevice& initRenderDevice,
                   const std::vector<std::uint8_t>& fragmentShaderData,
                   const std::vector<std::uint8_t>& vertexShaderData,
                   const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                   const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                   const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                   const std::string&,
                   const std::string&):
        RenderResource{initRenderDevice},
        vertexAttributes{initVertexAttributes},
        fragmentShaderConstantInfo{initFragmentShaderConstantInfo},
        vertexShaderConstantInfo{initVertexShaderConstantInfo}
    {
        ID3D11PixelShader* newPixelShader;
        if (const auto hr = renderDevice.getDevice()->CreatePixelShader(fragmentShaderData.data(), fragmentShaderData.size(), nullptr, &newPixelShader); FAILED(hr))
            throw std::system_error(hr, getErrorCategory(), "Failed to create a Direct3D 11 pixel shader");

        fragmentShader = newPixelShader;

        ID3D11VertexShader* newVertexShader;
        if (const auto hr = renderDevice.getDevice()->CreateVertexShader(vertexShaderData.data(), vertexShaderData.size(), nullptr, &newVertexShader); FAILED(hr))
            throw std::system_error(hr, getErrorCategory(), "Failed to create a Direct3D 11 vertex shader");

        vertexShader = newVertexShader;

        std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputElements;

        UINT offset = 0;

        for (const auto& vertexAttribute : RenderDevice::vertexAttributes)
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
                    case Vertex::Attribute::Usage::binormal:
                        semantic = "BINORMAL";
                        break;
                    case Vertex::Attribute::Usage::blendIndices:
                        semantic = "BLENDINDICES";
                        break;
                    case Vertex::Attribute::Usage::blendWeight:
                        semantic = "BLENDWEIGHT";
                        break;
                    case Vertex::Attribute::Usage::color:
                        semantic = "COLOR";
                        break;
                    case Vertex::Attribute::Usage::normal:
                        semantic = "NORMAL";
                        break;
                    case Vertex::Attribute::Usage::position:
                        semantic = "POSITION";
                        break;
                    case Vertex::Attribute::Usage::positionTransformed:
                        semantic = "POSITIONT";
                        break;
                    case Vertex::Attribute::Usage::pointSize:
                        semantic = "PSIZE";
                        break;
                    case Vertex::Attribute::Usage::tangent:
                        semantic = "TANGENT";
                        break;
                    case Vertex::Attribute::Usage::textureCoordinates0:
                        semantic = "TEXCOORD";
                        break;
                    case Vertex::Attribute::Usage::textureCoordinates1:
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

        if (const auto hr = renderDevice.getDevice()->CreateInputLayout(vertexInputElements.data(),
                                                                        static_cast<UINT>(vertexInputElements.size()),
                                                                        vertexShaderData.data(),
                                                                        vertexShaderData.size(),
                                                                        &newInputLayout); FAILED(hr))
            throw std::system_error(hr, getErrorCategory(), "Failed to create Direct3D 11 input layout for vertex shader");

        inputLayout = newInputLayout;

        if (!fragmentShaderConstantInfo.empty())
        {
            fragmentShaderConstantLocations.reserve(fragmentShaderConstantInfo.size());

            for (const auto& [name, dataType] : fragmentShaderConstantInfo)
            {
                (void)name;
                const auto size = getDataTypeSize(dataType);
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

        if (const auto hr = renderDevice.getDevice()->CreateBuffer(&fragmentShaderConstantBufferDesc, nullptr,
                                                                    &newFragmentShaderConstantBuffer); FAILED(hr))
            throw std::system_error(hr, getErrorCategory(), "Failed to create Direct3D 11 constant buffer");

        fragmentShaderConstantBuffer = newFragmentShaderConstantBuffer;

        if (!vertexShaderConstantInfo.empty())
        {
            vertexShaderConstantLocations.reserve(vertexShaderConstantInfo.size());

            for (const auto& info : vertexShaderConstantInfo)
            {
                const std::uint32_t size = getDataTypeSize(info.second);
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

        if (const auto hr = renderDevice.getDevice()->CreateBuffer(&vertexShaderConstantBufferDesc, nullptr,
                                                                    &newVertexShaderConstantBuffer); FAILED(hr))
            throw std::system_error(hr, getErrorCategory(), "Failed to create Direct3D 11 constant buffer");

        vertexShaderConstantBuffer = newVertexShaderConstantBuffer;
    }
}

#endif
