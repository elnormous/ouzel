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
        if (_programId) glDeleteProgram(_programId);
        if (_vertexShaderId) glDeleteShader(_vertexShaderId);
        if (_fragmentShaderId) glDeleteShader(_fragmentShaderId);
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
        
        _fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(_fragmentShaderId, 1, reinterpret_cast<const GLchar* const*>(&fragmentShader), reinterpret_cast<const GLint*>(&fragmentShaderSize));
        glCompileShader(_fragmentShaderId);
        
        if (checkShaderError(_fragmentShaderId))
        {
            return false;
        }
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
        {
            return false;
        }
        
        _vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(_vertexShaderId, 1, reinterpret_cast<const GLchar* const*>(&vertexShader), reinterpret_cast<const GLint*>(&vertexShaderSize));
        glCompileShader(_vertexShaderId);
        
        if (checkShaderError(_vertexShaderId))
        {
            return false;
        }
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
        {
            return false;
        }
        
        _programId = glCreateProgram();
        glAttachShader(_programId, _vertexShaderId);
        glAttachShader(_programId, _fragmentShaderId);
        glLinkProgram(_programId);
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
        {
            return false;
        }
        
        return true;
    }

    bool ShaderOGL::checkShaderError(GLuint shaderId)
    {
        GLint good;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &good);
        
        if (good == GL_FALSE)
        {
            GLint logLength = 0;
            glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
            char* logMessage = new char[logLength];
            glGetShaderInfoLog(shaderId, logLength, nullptr, logMessage);
            
            log("Shader error: %s", logMessage);
            
            delete [] logMessage;
            
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
