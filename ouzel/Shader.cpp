// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Shader.h"
#include "Utils.h"

namespace ouzel
{
    Shader::Shader(Renderer* renderer)
    {
        _renderer = renderer;
    }

    Shader::~Shader()
    {
        
    }
    
    bool Shader::loadFromFiles(const std::string& fragmentShader, const std::string& vertexShader)
    {
        _fragmentShaderFilename = fragmentShader;
        _vertexShaderFilename = vertexShader;
        
        return  true;
    }
    
    bool Shader::loadFromStrings(const std::string& fragmentShader, const std::string& vertexShader)
    {
        return  true;
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
