// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <fstream>
#include "ShaderOGL.h"
#include "Utils.h"

namespace ouzel
{
    ShaderOGL::ShaderOGL(const std::string& fragmentShader, const std::string& vertexShader, Renderer* renderer):
        Shader(fragmentShader, vertexShader, renderer)
    {
        std::ifstream fragmentShaderFile(getResourcePath(fragmentShader));
        std::string fragmentShaderCode;
        
        fragmentShaderFile.seekg(0, std::ios::end);
        fragmentShaderCode.reserve(fragmentShaderFile.tellg());
        fragmentShaderFile.seekg(0, std::ios::beg);
        
        fragmentShaderCode.assign((std::istreambuf_iterator<char>(fragmentShaderFile)),
                   std::istreambuf_iterator<char>());
        
        std::ifstream vertexShaderFile(getResourcePath(vertexShader));
        std::string vertexShaderCode;
        
        vertexShaderFile.seekg(0, std::ios::end);
        vertexShaderCode.reserve(vertexShaderFile.tellg());
        vertexShaderFile.seekg(0, std::ios::beg);
        
        vertexShaderCode.assign((std::istreambuf_iterator<char>(vertexShaderFile)),
                                  std::istreambuf_iterator<char>());
        
        GLboolean support;
        glGetBooleanv(GL_SHADER_COMPILER, &support);
        
        assert(support && "Shader compiler must be present");
        
        checkOpenGLErrors();
        
        _vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char* vertexShaderCodeStr = vertexShaderCode.c_str();
        glShaderSource(_vertexShader, 1, &vertexShaderCodeStr, NULL);
        glCompileShader(_vertexShader);
        
        checkShaderError(_vertexShader);
        
        _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragmentShaderCodeStr = fragmentShaderCode.c_str();
        glShaderSource(_fragmentShader, 1, &fragmentShaderCodeStr, NULL);
        glCompileShader(_fragmentShader);
        
        checkShaderError(_fragmentShader);
        
        _programId = glCreateProgram();
        glAttachShader(_programId, _vertexShader);
        glAttachShader(_programId, _fragmentShader);
        glLinkProgram(_programId);
        
        glUseProgram(_programId);
        
        checkOpenGLErrors();
    }

    ShaderOGL::~ShaderOGL()
    {
        
    }

    void ShaderOGL::checkShaderError(GLuint shader)
    {
        GLint good;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &good);
        
        if (good == GL_FALSE)
        {
            GLint logLength=0;
            char *logMessage=NULL;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            logMessage = (char*)malloc(logLength);
            glGetShaderInfoLog(shader, logLength, NULL, logMessage);
            
            log("Shader error: %s", logMessage);
            
            free(logMessage);
        }
    }
    
    uint32_t ShaderOGL::getPixelShaderConstantId(const std::string& name)
    {
        return glGetUniformLocation(_programId, name.c_str());
    }
    
    uint32_t ShaderOGL::getVertexShaderConstantId(const std::string& name)
    {
        return glGetUniformLocation(_programId, name.c_str());
    }
}
