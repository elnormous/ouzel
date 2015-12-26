// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <fstream>
#include "Shader.h"
#include "Engine.h"
#include "Renderer.h"
#include "FileSystem.h"
#include "Utils.h"

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
        
        std::ifstream fragmentShaderFile(_renderer->getEngine()->getFileSystem()->getPath(fragmentShader));
        
        if (!fragmentShaderFile)
        {
            log("Failed to open fragment shader file %s", fragmentShader.c_str());
            return false;
        }
        
        std::string fragmentShaderCode;
        
        fragmentShaderFile.seekg(0, std::ios::end);
        fragmentShaderCode.reserve(static_cast<size_t>(fragmentShaderFile.tellg()));
        fragmentShaderFile.seekg(0, std::ios::beg);
        
        fragmentShaderCode.assign((std::istreambuf_iterator<char>(fragmentShaderFile)),
                                  std::istreambuf_iterator<char>());
        
        std::ifstream vertexShaderFile(_renderer->getEngine()->getFileSystem()->getPath(vertexShader));
        
        if (!vertexShaderFile)
        {
            log("Failed to open vertex shader file %s", fragmentShader.c_str());
            return false;
        }
        
        std::string vertexShaderCode;
        
        vertexShaderFile.seekg(0, std::ios::end);
        vertexShaderCode.reserve(static_cast<size_t>(vertexShaderFile.tellg()));
        vertexShaderFile.seekg(0, std::ios::beg);
        
        vertexShaderCode.assign((std::istreambuf_iterator<char>(vertexShaderFile)),
                                std::istreambuf_iterator<char>());
        
        return initFromStrings(fragmentShaderCode, vertexShaderCode);
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
