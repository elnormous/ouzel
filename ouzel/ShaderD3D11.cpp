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
        
        return true;
    }
	
    uint32_t ShaderD3D11::getPixelShaderConstantId(const std::string& name)
    {
        return 0;
    }
    
    uint32_t ShaderD3D11::getVertexShaderConstantId(const std::string& name)
    {
        return 0;
    }
}
