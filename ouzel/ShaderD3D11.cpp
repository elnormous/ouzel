// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <fstream>
#include "ShaderD3D11.h"
#include "Engine.h"
#include "Renderer.h"
#include "FileSystem.h"
#include "Utils.h"

namespace ouzel
{
    const D3D11_INPUT_ELEMENT_DESC g_VertexInputElements[] =
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
        
    }
    
    bool ShaderD3D11::initFromStrings(const std::string& fragmentShader, const std::string& vertexShader)
    {
        if (!Shader::initFromFiles(fragmentShader, vertexShader))
        {
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
