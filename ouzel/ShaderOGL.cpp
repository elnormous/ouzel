// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Engine.h"
#include "RendererOGL.h"
#include "FileSystem.h"
#include "ShaderOGL.h"
#include "Utils.h"

namespace ouzel
{
    ShaderOGL::ShaderOGL():
        Shader()
    {
    }

    ShaderOGL::~ShaderOGL()
    {
        
    }
    
    bool ShaderOGL::initFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize)
    {
        if (!Shader::initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize))
        {
            return false;
        }
        
        GLboolean support;
        glGetBooleanv(GL_SHADER_COMPILER, &support);
        
        if (!support)
        {
            log("No shader compiler present");
            return false;
        }
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
        {
            return false;
        }
        
        _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(_fragmentShader, 1, reinterpret_cast<const GLchar* const*>(&fragmentShader), reinterpret_cast<const GLint*>(&fragmentShaderSize));
        glCompileShader(_fragmentShader);
        
        if (checkShaderError(_fragmentShader))
        {
            return false;
        }
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
        {
            return false;
        }
        
        _vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(_vertexShader, 1, reinterpret_cast<const GLchar* const*>(&vertexShader), reinterpret_cast<const GLint*>(&vertexShaderSize));
        glCompileShader(_vertexShader);
        
        if (checkShaderError(_vertexShader))
        {
            return false;
        }
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
        {
            return false;
        }
        
        _programId = glCreateProgram();
        glAttachShader(_programId, _vertexShader);
        glAttachShader(_programId, _fragmentShader);
        glLinkProgram(_programId);
        
        glUseProgram(_programId);
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
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
    
    bool ShaderOGL::setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
    {
        glUniform3fv(index, static_cast<GLsizei>(vectors.size()), reinterpret_cast<const float*>(vectors.data()));
        return true;
    }
    
    bool ShaderOGL::setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
    {
        glUniform4fv(index, static_cast<GLsizei>(vectors.size()), reinterpret_cast<const float*>(vectors.data()));
        return true;
    }
    
    bool ShaderOGL::setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
    {
        glUniformMatrix4fv(index, static_cast<GLsizei>(matrices.size()), GL_FALSE, reinterpret_cast<const float*>(matrices.data()));
        return true;
    }
    
    uint32_t ShaderOGL::getVertexShaderConstantId(const std::string& name)
    {
        return glGetUniformLocation(_programId, name.c_str());
    }
    
    bool ShaderOGL::setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
    {
        glUniform3fv(index, static_cast<GLsizei>(vectors.size()), reinterpret_cast<const float*>(vectors.data()));
        return true;
    }
    
    bool ShaderOGL::setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
    {
        glUniform4fv(index, static_cast<GLsizei>(vectors.size()), reinterpret_cast<const float*>(vectors.data()));
        return true;
    }
    
    bool ShaderOGL::setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
    {
        glUniformMatrix4fv(index, static_cast<GLsizei>(matrices.size()), GL_FALSE, reinterpret_cast<const float*>(matrices.data()));
        return true;
    }
}
