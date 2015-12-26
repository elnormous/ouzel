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
    ShaderD3D11::ShaderD3D11(Renderer* renderer):
        Shader(renderer)
    {
    }

    ShaderD3D11::~ShaderD3D11()
    {
        
    }
    
    bool ShaderD3D11::initFromFiles(const std::string& fragmentShader, const std::string& vertexShader)
    {
        if (!Shader::initFromFiles(fragmentShader, vertexShader))
        {
            return false;
        }
        
        std::ifstream fragmentShaderFile(_renderer->getEngine()->getFileSystem()->getPath(fragmentShader));
        std::string fragmentShaderCode;
        
        fragmentShaderFile.seekg(0, std::ios::end);
        fragmentShaderCode.reserve(fragmentShaderFile.tellg());
        fragmentShaderFile.seekg(0, std::ios::beg);
        
        fragmentShaderCode.assign((std::istreambuf_iterator<char>(fragmentShaderFile)),
                                  std::istreambuf_iterator<char>());
        
        std::ifstream vertexShaderFile(_renderer->getEngine()->getFileSystem()->getPath(vertexShader));
        std::string vertexShaderCode;
        
        vertexShaderFile.seekg(0, std::ios::end);
        vertexShaderCode.reserve(vertexShaderFile.tellg());
        vertexShaderFile.seekg(0, std::ios::beg);
        
        vertexShaderCode.assign((std::istreambuf_iterator<char>(vertexShaderFile)),
                                std::istreambuf_iterator<char>());
        
        return initFromStrings(fragmentShaderCode, vertexShaderCode);
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
