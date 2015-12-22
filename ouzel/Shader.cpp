// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Shader.h"
#include "Renderer.h"

namespace ouzel
{
    Shader::Shader(Renderer* renderer):
        _renderer(renderer)
    {
        
    }

    Shader::~Shader()
    {
        if (_renderer->getActiveShader() == this)
        {
            _renderer->activateShader(nullptr);
        }
    }
    
    bool Shader::initFromFiles(const std::string& fragmentShader, const std::string& vertexShader)
    {
        _fragmentShaderFilename = fragmentShader;
        _vertexShaderFilename = vertexShader;
        
        return  true;
    }
    
    bool Shader::initFromStrings(const std::string& fragmentShader, const std::string& vertexShader)
    {
        return  true;
    }
    
    uint32_t Shader::getPixelShaderConstantId(const std::string& name)
    {
        return 0;
    }
    
    bool Shader::setPixelShaderConstant(uint32_t index, const Vector3* vectors, uint32_t count)
    {
        return true;
    }
    
    bool Shader::setPixelShaderConstant(uint32_t index, const Vector4* vectors, uint32_t count)
    {
        return true;
    }
    
    bool Shader::setPixelShaderConstant(uint32_t index, const Matrix4* matrixes, uint32_t count)
    {
        return true;
    }
    
    uint32_t Shader::getVertexShaderConstantId(const std::string& name)
    {
        return 0;
    }
    
    bool Shader::setVertexShaderConstant(uint32_t index, const Vector3* vectors, uint32_t count)
    {
        return true;
    }
    
    bool Shader::setVertexShaderConstant(uint32_t index, const Vector4* vectors, uint32_t count)
    {
        return true;
    }
    
    bool Shader::setVertexShaderConstant(uint32_t index, const Matrix4* matrixes, uint32_t count)
    {
        return true;
    }
}
