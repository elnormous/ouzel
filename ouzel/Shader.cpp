//
//  Shader.cpp
//  Ouzel
//
//  Created by Elviss Strazdins on 14/03/15.
//  Copyright (c) 2015 Elviss. All rights reserved.
//

#include "Shader.h"
#include "Utils.h"

namespace ouzel
{
    Shader::Shader(const std::string& fragmentShader, const std::string& vertexShader)
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
        
        _program = glCreateProgram();
        glAttachShader(_program, _vertexShader);
        glAttachShader(_program, _fragmentShader);
        glLinkProgram(_program);
        
        glUseProgram(_program);
        
        checkOpenGLErrors();
    }

    Shader::~Shader()
    {
        
    }

    void Shader::checkShaderError(GLuint shader)
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
            
            printf("Shader error: %s\n", logMessage);
            
            free(logMessage);
        }
    }
}
