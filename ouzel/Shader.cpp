// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <fstream>
#include "Shader.h"
#include "Engine.h"
#include "Renderer.h"
#include "FileSystem.h"
#include "Utils.h"

namespace ouzel
{
    Shader::Shader()
    {
        
    }

    Shader::~Shader()
    {

    }
    
    bool Shader::initFromFiles(const std::string& fragmentShader, const std::string& vertexShader, uint32_t vertexAttributes)
    {
        _fragmentShaderFilename = fragmentShader;
        _vertexShaderFilename = vertexShader;
        
        std::ifstream fragmentShaderFile(Engine::getInstance()->getFileSystem()->getPath(fragmentShader));
        
        if (!fragmentShaderFile)
        {
            log("Failed to open fragment shader file %s", fragmentShader.c_str());
            return false;
        }
        
        fragmentShaderFile.seekg(0, std::ios::end);
        size_t fragmentShaderSize = static_cast<size_t>(fragmentShaderFile.tellg());
        fragmentShaderFile.seekg(0, std::ios::beg);
        
        std::vector<char> fragmentShaderBuffer(fragmentShaderSize);
        
        fragmentShaderFile.read(fragmentShaderBuffer.data(), fragmentShaderSize);
        
        std::ifstream vertexShaderFile(Engine::getInstance()->getFileSystem()->getPath(vertexShader));
        
        if (!vertexShaderFile)
        {
            log("Failed to open vertex shader file %s", fragmentShader.c_str());
            return false;
        }
        
        std::string vertexShaderCode;
        
        vertexShaderFile.seekg(0, std::ios::end);
        size_t vertexShaderSize = static_cast<size_t>(vertexShaderFile.tellg());
        vertexShaderFile.seekg(0, std::ios::beg);
        
        std::vector<char> vertexShaderBuffer(vertexShaderSize);
        
        vertexShaderFile.read(vertexShaderBuffer.data(), vertexShaderSize);
        
        return initFromBuffers(reinterpret_cast<const uint8_t*>(fragmentShaderBuffer.data()), static_cast<int32_t>(fragmentShaderSize),
                               reinterpret_cast<const uint8_t*>(vertexShaderBuffer.data()), static_cast<int32_t>(vertexShaderSize), vertexAttributes);
    }
    
    bool Shader::initFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes)
    {
        OUZEL_UNUSED(fragmentShader);
        OUZEL_UNUSED(fragmentShaderSize);
        OUZEL_UNUSED(vertexShader);
        OUZEL_UNUSED(vertexShaderSize);
        
        _vertexAttributes = vertexAttributes;
        
        return  true;
    }
    
    uint32_t Shader::getPixelShaderConstantId(const std::string& name)
    {
        OUZEL_UNUSED(name);
        
        return 0;
    }
    
    bool Shader::setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
    {
        OUZEL_UNUSED(index);
        OUZEL_UNUSED(vectors);
        
        return true;
    }
    
    bool Shader::setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
    {
        OUZEL_UNUSED(index);
        OUZEL_UNUSED(vectors);
        
        return true;
    }
    
    bool Shader::setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
    {
        OUZEL_UNUSED(index);
        OUZEL_UNUSED(matrices);
        
        return true;
    }
    
    uint32_t Shader::getVertexShaderConstantId(const std::string& name)
    {
        OUZEL_UNUSED(name);
        
        return 0;
    }
    
    bool Shader::setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
    {
        OUZEL_UNUSED(index);
        OUZEL_UNUSED(vectors);
        
        return true;
    }
    
    bool Shader::setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
    {
        OUZEL_UNUSED(index);
        OUZEL_UNUSED(vectors);
        
        return true;
    }
    
    bool Shader::setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
    {
        OUZEL_UNUSED(index);
        OUZEL_UNUSED(matrices);
        
        return true;
    }
}
