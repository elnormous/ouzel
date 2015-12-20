// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Shader.h"
#include "Utils.h"

namespace ouzel
{
    Shader::Shader(const std::string& fragmentShader, const std::string& vertexShader, Renderer* renderer)
    {
        _fragmentShader = fragmentShader;
        _vertexShader = vertexShader;
        _renderer = renderer;
    }

    Shader::~Shader()
    {
        
    }
    
    uint32_t Shader::getPixelShaderConstantId(const std::string& name)
    {
        return 0;
    }
    
    uint32_t Shader::getVertexShaderConstantId(const std::string& name)
    {
        return 0;
    }
}
