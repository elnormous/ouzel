// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Engine.h"
#include "RendererOGL.h"
#include "FileSystem.h"
#include "ShaderOGL.h"
#include "Utils.h"

namespace ouzel
{
    ShaderOGL::ShaderOGL(const std::string& fragmentShader, const std::string& vertexShader, Renderer* renderer):
        Shader(renderer)
    {
    }

    ShaderOGL::~ShaderOGL()
    {
        
    }
    
    bool ShaderOGL::initFromStrings(const std::string& fragmentShader, const std::string& vertexShader)
    {
        if (!Shader::initFromStrings(fragmentShader, vertexShader))
        {
            return false;
        }
        
        GLboolean support;
        glGetBooleanv(GL_SHADER_COMPILER, &support);
        
        if (!support)
        {
            log("Shader compiler must be present");
            return false;
        }
        
        if (static_cast<RendererOGL*>(_renderer)->checkOpenGLErrors())
        {
            return false;
        }
        
        _vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char* vertexShaderCodeStr = vertexShader.c_str();
        glShaderSource(_vertexShader, 1, &vertexShaderCodeStr, NULL);
        glCompileShader(_vertexShader);
        
        if (checkShaderError(_vertexShader))
        {
            return false;
        }
        
        _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragmentShaderCodeStr = fragmentShader.c_str();
        glShaderSource(_fragmentShader, 1, &fragmentShaderCodeStr, NULL);
        glCompileShader(_fragmentShader);
        
        if (checkShaderError(_fragmentShader))
        {
            return false;
        }
        
        _programId = glCreateProgram();
        glAttachShader(_programId, _vertexShader);
        glAttachShader(_programId, _fragmentShader);
        glLinkProgram(_programId);
        
        glUseProgram(_programId);
        
        if (static_cast<RendererOGL*>(_renderer)->checkOpenGLErrors())
        {
            return false;
        }
        
        return true;
    }

    bool ShaderOGL::checkShaderError(GLuint shader)
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
            
            return true;
        }
        
        return false;
    }
    
    uint32_t ShaderOGL::getPixelShaderConstantId(const std::string& name)
    {
        return glGetUniformLocation(_programId, name.c_str());
    }
    
    bool ShaderOGL::setPixelShaderConstant(uint32_t index, const Vector3* vectors, uint32_t count)
    {
        glUniform3fv(index, count, reinterpret_cast<const float*>(vectors));
        return true;
    }
    
    bool ShaderOGL::setPixelShaderConstant(uint32_t index, const Vector4* vectors, uint32_t count)
    {
        glUniform4fv(index, count, reinterpret_cast<const float*>(vectors));
        return true;
    }
    
    bool ShaderOGL::setPixelShaderConstant(uint32_t index, const Matrix4* matrixes, uint32_t count)
    {
        glUniformMatrix4fv(index, count, GL_FALSE, reinterpret_cast<const float*>(matrixes));
        return true;
    }
    
    uint32_t ShaderOGL::getVertexShaderConstantId(const std::string& name)
    {
        return glGetUniformLocation(_programId, name.c_str());
    }
    
    bool ShaderOGL::setVertexShaderConstant(uint32_t index, const Vector3* vectors, uint32_t count)
    {
        glUniform3fv(index, count, reinterpret_cast<const float*>(vectors));
        return true;
    }
    
    bool ShaderOGL::setVertexShaderConstant(uint32_t index, const Vector4* vectors, uint32_t count)
    {
        glUniform4fv(index, count, reinterpret_cast<const float*>(vectors));
        return true;
    }
    
    bool ShaderOGL::setVertexShaderConstant(uint32_t index, const Matrix4* matrixes, uint32_t count)
    {
        glUniformMatrix4fv(index, count, GL_FALSE, reinterpret_cast<const float*>(matrixes));
        return true;
    }
}
