// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <fstream>
#include "ShaderD3D11.h"
#include "Engine.h"
#include "RendererD3D11.h"
#include "FileSystem.h"
#include "Utils.h"

namespace ouzel
{
    const D3D11_INPUT_ELEMENT_DESC VERTEX_INPUT_ELEMENTS[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    ShaderD3D11::ShaderD3D11(Renderer* renderer):
        Shader(renderer)
    {
    }

    ShaderD3D11::~ShaderD3D11()
    {
        if (_pixelShader) _pixelShader->Release();
        if (_vertexShader) _vertexShader->Release();
        if (_inputLayout) _inputLayout->Release();

        if (_pixelShaderConstantBuffer) _pixelShaderConstantBuffer->Release();
        if (_vertexShaderConstantBuffer) _vertexShaderConstantBuffer->Release();
    }
    
    bool ShaderD3D11::initFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize)
    {
        if (!Shader::initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize))
        {
            return false;
        }

        RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(_renderer);

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

        hr = rendererD3D11->getDevice()->CreateInputLayout(
            VERTEX_INPUT_ELEMENTS,
            static_cast<UINT>(sizeof(VERTEX_INPUT_ELEMENTS) / sizeof(D3D11_INPUT_ELEMENT_DESC)),
            vertexShader,
            vertexShaderSize,
            &_inputLayout);

        if (FAILED(hr) || !_inputLayout)
        {
            log("Failed to create D3D11 input layout for vertex shader");
            return false;
        }
        
        D3D11_BUFFER_DESC pixelShaderConstantBufferDesc;
        memset(&pixelShaderConstantBufferDesc, 0, sizeof(pixelShaderConstantBufferDesc));

        pixelShaderConstantBufferDesc.ByteWidth = (UINT)sizeof(Matrix4);
        pixelShaderConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        pixelShaderConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        pixelShaderConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        hr = rendererD3D11->getDevice()->CreateBuffer(&pixelShaderConstantBufferDesc, nullptr, &_pixelShaderConstantBuffer);
        if (FAILED(hr) || !_pixelShaderConstantBuffer)
        {
            log("Failed to create D3D11 constant buffer");
            return false;
        }

        D3D11_BUFFER_DESC vertexShaderConstantBufferDesc;
        memset(&vertexShaderConstantBufferDesc, 0, sizeof(vertexShaderConstantBufferDesc));

        vertexShaderConstantBufferDesc.ByteWidth = (UINT)sizeof(Matrix4);
        vertexShaderConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexShaderConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        vertexShaderConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        hr = rendererD3D11->getDevice()->CreateBuffer(&vertexShaderConstantBufferDesc, nullptr, &_vertexShaderConstantBuffer);
        if (FAILED(hr) || !_vertexShaderConstantBuffer)
        {
            log("Failed to create D3D11 constant buffer");
            return false;
        }

        return true;
    }
	
    uint32_t ShaderD3D11::getPixelShaderConstantId(const std::string& name)
    {
        log("getPixelShaderConstantId not available for D3D11");
        return 0;
    }

    bool ShaderD3D11::setPixelShaderConstant(uint32_t index, const Vector3* vectors, uint32_t count)
    {
        uploadData(_pixelShaderConstantBuffer, vectors, count * sizeof(Vector3));

        return true;
    }

    bool ShaderD3D11::setPixelShaderConstant(uint32_t index, const Vector4* vectors, uint32_t count)
    {
        uploadData(_pixelShaderConstantBuffer, vectors, count * sizeof(Vector4));

        return true;
    }

    bool ShaderD3D11::setPixelShaderConstant(uint32_t index, const Matrix4* matrices, uint32_t count)
    {
        uploadData(_pixelShaderConstantBuffer, matrices, count * sizeof(Matrix4));

        return true;
    }
    
    uint32_t ShaderD3D11::getVertexShaderConstantId(const std::string& name)
    {
        log("getVertexShaderConstantId not available for D3D11");
        return 0;
    }

    bool ShaderD3D11::setVertexShaderConstant(uint32_t index, const Vector3* vectors, uint32_t count)
    {
        uploadData(_vertexShaderConstantBuffer, vectors, count * sizeof(Vector3));

        return true;
    }

    bool ShaderD3D11::setVertexShaderConstant(uint32_t index, const Vector4* vectors, uint32_t count)
    {
        uploadData(_vertexShaderConstantBuffer, vectors, count * sizeof(Vector4));

        return true;
    }

    bool ShaderD3D11::setVertexShaderConstant(uint32_t index, const Matrix4* matrices, uint32_t count)
    {
        uploadData(_vertexShaderConstantBuffer, matrices, count * sizeof(Matrix4));

        return true;
    }

    bool ShaderD3D11::uploadData(ID3D11Buffer* buffer, const void* data, uint32_t size)
    {
        RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(_renderer);

        D3D11_MAPPED_SUBRESOURCE mappedSubResource;
        HRESULT hr = rendererD3D11->getContext()->Map(_pixelShaderConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
        if (FAILED(hr))
        {
            log("Failed to lock D3D11 buffer");
            return false;
        }

        memcpy(mappedSubResource.pData, data, size);

        rendererD3D11->getContext()->Unmap(_pixelShaderConstantBuffer, 0);

        return true;
    }
}
