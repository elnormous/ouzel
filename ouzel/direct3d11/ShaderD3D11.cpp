// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <fstream>
#include "ShaderD3D11.h"
#include "Engine.h"
#include "RendererD3D11.h"
#include "FileSystem.h"
#include "Utils.h"

namespace ouzel
{
    namespace video
    {
        ShaderD3D11::ShaderD3D11()
        {
        }

        ShaderD3D11::~ShaderD3D11()
        {
            destroy();
        }

        void ShaderD3D11::destroy()
        {
            if (_pixelShader)
            {
                _pixelShader->Release();
                _pixelShader = nullptr;
            }

            if (_vertexShader)
            {
                _vertexShader->Release();
                _vertexShader = nullptr;
            }

            if (_inputLayout)
            {
                _inputLayout->Release();
                _inputLayout = nullptr;
            }

            if (_pixelShaderConstantBuffer)
            {
                _pixelShaderConstantBuffer->Release();
                _pixelShaderConstantBuffer = nullptr;
            }

            if (_vertexShaderConstantBuffer)
            {
                _vertexShaderConstantBuffer->Release();
                _vertexShaderConstantBuffer = nullptr;
            }
        }

        bool ShaderD3D11::initFromBuffers(const uint8_t* fragmentShader,
                                          uint32_t fragmentShaderSize,
                                          const uint8_t* vertexShader,
                                          uint32_t vertexShaderSize,
                                          uint32_t vertexAttributes,
                                          const std::string& fragmentShaderFunction,
                                          const std::string& vertexShaderFunction)
        {
            if (!Shader::initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize, vertexAttributes, fragmentShaderFunction, vertexShaderFunction))
            {
                return false;
            }

            destroy();

            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            HRESULT hr = rendererD3D11->getDevice()->CreatePixelShader(fragmentShader, fragmentShaderSize, NULL, &_pixelShader);
            if (FAILED(hr) || !_pixelShader)
            {
                log("Failed to create a D3D11 pixel shader");
                return false;
            }

            hr = rendererD3D11->getDevice()->CreateVertexShader(vertexShader, vertexShaderSize, NULL, &_vertexShader);
            if (FAILED(hr) || !_vertexShader)
            {
                log("Failed to create a D3D11 vertex shader");
                return false;
            }

            std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputElements;

            UINT offset = 0;

            if (vertexAttributes & VERTEX_POSITION)
            {
                vertexInputElements.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                offset += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_COLOR)
            {
                vertexInputElements.push_back({ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                offset += 4 * sizeof(uint8_t);
            }

            if (vertexAttributes & VERTEX_NORMAL)
            {
                vertexInputElements.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                offset += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD0)
            {
                vertexInputElements.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                offset += 2 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD1)
            {
                vertexInputElements.push_back({ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                offset += 2 * sizeof(float);
            }

            hr = rendererD3D11->getDevice()->CreateInputLayout(
                vertexInputElements.data(),
                static_cast<UINT>(vertexInputElements.size()),
                vertexShader,
                vertexShaderSize,
                &_inputLayout);

            if (FAILED(hr) || !_inputLayout)
            {
                log("Failed to create D3D11 input layout for vertex shader");
                return false;
            }

            if (!createPixelShaderConstantBuffer(sizeof(Matrix4)))
            {
                return false;
            }

            if (!createVertexShaderConstantBuffer(sizeof(Matrix4)))
            {
                return false;
            }

            return true;
        }

        uint32_t ShaderD3D11::getPixelShaderConstantId(const std::string& name)
        {
            log("getPixelShaderConstantId not available for D3D11");
            return 0;
        }

        bool ShaderD3D11::setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
        {
            uint32_t size = index + vectorDataSize(vectors);
            if (size > _pixelShaderData.size())
            {
                if (_pixelShaderConstantBuffer) _pixelShaderConstantBuffer->Release();
                createPixelShaderConstantBuffer(size);
            }

            memcpy(_pixelShaderData.data() + index, vectors.data(), vectorDataSize(vectors));
            return uploadData(_pixelShaderConstantBuffer, _pixelShaderData.data(), _pixelShaderData.size());
        }

        bool ShaderD3D11::setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
        {
            uint32_t size = index + vectorDataSize(vectors);
            if (size > _pixelShaderData.size())
            {
                if (_pixelShaderConstantBuffer) _pixelShaderConstantBuffer->Release();
                createPixelShaderConstantBuffer(size);
            }

            memcpy(_pixelShaderData.data() + index, vectors.data(), vectorDataSize(vectors));
            return uploadData(_pixelShaderConstantBuffer, _pixelShaderData.data(), _pixelShaderData.size());
        }

        bool ShaderD3D11::setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
        {
            uint32_t size = index + vectorDataSize(matrices);
            if (size > _pixelShaderData.size())
            {
                if (_pixelShaderConstantBuffer) _pixelShaderConstantBuffer->Release();
                createPixelShaderConstantBuffer(size);
            }

            memcpy(_pixelShaderData.data() + index, matrices.data(), vectorDataSize(matrices));
            return uploadData(_pixelShaderConstantBuffer, _pixelShaderData.data(), _pixelShaderData.size());
        }

        uint32_t ShaderD3D11::getVertexShaderConstantId(const std::string& name)
        {
            log("getVertexShaderConstantId not available for D3D11");
            return 0;
        }

        bool ShaderD3D11::setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
        {
            uint32_t size = index + vectorDataSize(vectors);
            if (size > _vertexShaderData.size())
            {
                if (_vertexShaderConstantBuffer) _vertexShaderConstantBuffer->Release();
                createVertexShaderConstantBuffer(size);
            }

            memcpy(_vertexShaderData.data() + index, vectors.data(), vectorDataSize(vectors));
            return uploadData(_vertexShaderConstantBuffer, _vertexShaderData.data(), _vertexShaderData.size());
        }

        bool ShaderD3D11::setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
        {
            uint32_t size = index + vectorDataSize(vectors);
            if (size > _vertexShaderData.size())
            {
                if (_vertexShaderConstantBuffer) _vertexShaderConstantBuffer->Release();
                createVertexShaderConstantBuffer(size);
            }

            memcpy(_vertexShaderData.data() + index, vectors.data(), vectorDataSize(vectors));
            return uploadData(_vertexShaderConstantBuffer, _vertexShaderData.data(), _vertexShaderData.size());
        }

        bool ShaderD3D11::setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
        {
            uint32_t size = index + vectorDataSize(matrices);
            if (size > _vertexShaderData.size())
            {
                if (_vertexShaderConstantBuffer) _vertexShaderConstantBuffer->Release();
                createVertexShaderConstantBuffer(size);
            }

            memcpy(_vertexShaderData.data() + index, matrices.data(), vectorDataSize(matrices));
            return uploadData(_vertexShaderConstantBuffer, _vertexShaderData.data(), _vertexShaderData.size());
        }

        bool ShaderD3D11::createPixelShaderConstantBuffer(uint32_t size)
        {
            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            D3D11_BUFFER_DESC pixelShaderConstantBufferDesc;
            memset(&pixelShaderConstantBufferDesc, 0, sizeof(pixelShaderConstantBufferDesc));

            pixelShaderConstantBufferDesc.ByteWidth = static_cast<UINT>(size);
            pixelShaderConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            pixelShaderConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            pixelShaderConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&pixelShaderConstantBufferDesc, nullptr, &_pixelShaderConstantBuffer);
            if (FAILED(hr) || !_pixelShaderConstantBuffer)
            {
                log("Failed to create D3D11 constant buffer");
                return false;
            }

            _pixelShaderData.resize(size);

            return true;
        }

        bool ShaderD3D11::createVertexShaderConstantBuffer(uint32_t size)
        {
            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            D3D11_BUFFER_DESC vertexShaderConstantBufferDesc;
            memset(&vertexShaderConstantBufferDesc, 0, sizeof(vertexShaderConstantBufferDesc));

            vertexShaderConstantBufferDesc.ByteWidth = static_cast<UINT>(size);
            vertexShaderConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            vertexShaderConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            vertexShaderConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&vertexShaderConstantBufferDesc, nullptr, &_vertexShaderConstantBuffer);
            if (FAILED(hr) || !_vertexShaderConstantBuffer)
            {
                log("Failed to create D3D11 constant buffer");
                return false;
            }

            _vertexShaderData.resize(size);

            return true;
        }

        bool ShaderD3D11::uploadData(ID3D11Buffer* buffer, const void* data, uint32_t size)
        {
            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            D3D11_MAPPED_SUBRESOURCE mappedSubresource;
            HRESULT hr = rendererD3D11->getContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
            if (FAILED(hr))
            {
                log("Failed to lock D3D11 buffer");
                return false;
            }

            memcpy(mappedSubresource.pData, data, size);

            rendererD3D11->getContext()->Unmap(buffer, 0);

            return true;
        }
    } // namespace video
} // namespace ouzel
