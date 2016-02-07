// Copyright (C) 2016 Elviss Strazdins
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
        clean();
    }
    
    void ShaderOGL::clean()
    {
        if (_programId) glDeleteProgram(_programId);
        if (_vertexShaderId) glDeleteShader(_vertexShaderId);
        if (_fragmentShaderId) glDeleteShader(_fragmentShaderId);
    }
    
    bool ShaderOGL::initFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes)
    {
        if (!Shader::initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize, vertexAttributes))
        {
            return false;
        }
        
        clean();
        
        GLboolean support;
        glGetBooleanv(GL_SHADER_COMPILER, &support);
        
        if (!support)
        {
            log("No shader compiler present");
            return false;
        }
        
        if (std::static_pointer_cast<RendererOGL>(Engine::getInstance()->getRenderer())->checkOpenGLErrors())
        {
            return false;
        }
        
        _fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(_fragmentShaderId, 1, reinterpret_cast<const GLchar**>(&fragmentShader), reinterpret_cast<const GLint*>(&fragmentShaderSize));
        glCompileShader(_fragmentShaderId);
        
        printShaderMessage(_fragmentShaderId);
        
        GLint status;
        glGetShaderiv(_fragmentShaderId, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            return false;
        }
        
        if (std::static_pointer_cast<RendererOGL>(Engine::getInstance()->getRenderer())->checkOpenGLErrors())
        {
            return false;
        }
        
        _vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(_vertexShaderId, 1, reinterpret_cast<const GLchar**>(&vertexShader), reinterpret_cast<const GLint*>(&vertexShaderSize));
        glCompileShader(_vertexShaderId);
        
        printShaderMessage(_fragmentShaderId);
        
        glGetShaderiv(_vertexShaderId, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            return false;
        }
        
        _programId = glCreateProgram();
        glAttachShader(_programId, _vertexShaderId);
        glAttachShader(_programId, _fragmentShaderId);
        
        GLuint index = 0;
        
        if (vertexAttributes & VERTEX_POSITION)
        {
            glBindAttribLocation(_programId, index, "in_Position");
            index++;
        }
        
        if (vertexAttributes & VERTEX_COLOR)
        {
            glBindAttribLocation(_programId, index, "in_Color");
            index++;
        }
        
        if (vertexAttributes & VERTEX_NORMAL)
        {
            glBindAttribLocation(_programId, index, "in_Normal");
            index++;
        }
        
        if (vertexAttributes & VERTEX_TEXCOORD0)
        {
            glBindAttribLocation(_programId, index, "in_TexCoord0");
            index++;
        }
        
        if (vertexAttributes & VERTEX_TEXCOORD1)
        {
            glBindAttribLocation(_programId, index, "in_TexCoord1");
            index++;
        }
        
        glLinkProgram(_programId);
        
        printProgramMessage(_programId);
        
        glGetProgramiv(_programId, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            return false;
        }
        
        if (std::static_pointer_cast<RendererOGL>(Engine::getInstance()->getRenderer())->checkOpenGLErrors())
        {
            return false;
        }
        
        GLint oldProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);
        
        glUseProgram(_programId);
        
        GLint texture0Location = glGetUniformLocation(_programId, "texture0");
        if (texture0Location != -1) glUniform1i(texture0Location, 0);
        
        GLint texture1Location = glGetUniformLocation(_programId, "texture1");
        if (texture1Location != -1) glUniform1i(texture1Location, 1);
        
        glUseProgram(oldProgram);
        
        if (std::static_pointer_cast<RendererOGL>(Engine::getInstance()->getRenderer())->checkOpenGLErrors())
        {
            return false;
        }
        
        glDetachShader(_programId, _vertexShaderId);
        glDeleteShader(_vertexShaderId);
        _vertexShaderId = 0;
        
        glDetachShader(_programId, _fragmentShaderId);
        glDeleteShader(_fragmentShaderId);
        _fragmentShaderId = 0;
        
        if (std::static_pointer_cast<RendererOGL>(Engine::getInstance()->getRenderer())->checkOpenGLErrors())
        {
            return false;
        }
        
        return true;
    }

    void ShaderOGL::printShaderMessage(GLuint shaderId)
    {
        GLint logLength = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
        
        if (logLength > 0)
        {
            char* logMessage = new char[logLength];
            glGetShaderInfoLog(shaderId, logLength, nullptr, logMessage);
            
            log("%s", logMessage);
            
            delete [] logMessage;
        }
    }
    
    void ShaderOGL::printProgramMessage(GLuint programId)
    {
        GLint logLength = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
        
        if (logLength > 0)
        {
            char* logMessage = new char[logLength];
            glGetProgramInfoLog(programId, logLength, nullptr, logMessage);
            
            log("%s", logMessage);
            
            delete [] logMessage;
        }
    }
    
    uint32_t ShaderOGL::getPixelShaderConstantId(const std::string& name)
    {
        return glGetUniformLocation(_programId, name.c_str());
    }
    
    bool ShaderOGL::setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
    {
        GLint oldProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);
        
        if (oldProgram != _programId) glUseProgram(_programId);
        glUniform3fv(index, static_cast<GLsizei>(vectors.size()), reinterpret_cast<const float*>(vectors.data()));
        
        if (oldProgram != _programId) glUseProgram(oldProgram);
        
        return true;
    }
    
    bool ShaderOGL::setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
    {
        GLint oldProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);
        
        if (oldProgram != _programId) glUseProgram(_programId);
        glUniform4fv(index, static_cast<GLsizei>(vectors.size()), reinterpret_cast<const float*>(vectors.data()));
        
        if (oldProgram != _programId) glUseProgram(oldProgram);
        
        return true;
    }
    
    bool ShaderOGL::setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
    {
        GLint oldProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);
        
        if (oldProgram != _programId) glUseProgram(_programId);
        glUniformMatrix4fv(index, static_cast<GLsizei>(matrices.size()), GL_FALSE, reinterpret_cast<const float*>(matrices.data()));
        
        if (oldProgram != _programId) glUseProgram(oldProgram);
        
        return true;
    }
    
    uint32_t ShaderOGL::getVertexShaderConstantId(const std::string& name)
    {
        return glGetUniformLocation(_programId, name.c_str());
    }
    
    bool ShaderOGL::setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
    {
        GLint oldProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);
        
        if (oldProgram != _programId) glUseProgram(_programId);
        glUniform3fv(index, static_cast<GLsizei>(vectors.size()), reinterpret_cast<const float*>(vectors.data()));
        
        if (oldProgram != _programId) glUseProgram(oldProgram);
        
        return true;
    }
    
    bool ShaderOGL::setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
    {
        GLint oldProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);
        
        if (oldProgram != _programId) glUseProgram(_programId);
        glUniform4fv(index, static_cast<GLsizei>(vectors.size()), reinterpret_cast<const float*>(vectors.data()));
        
        if (oldProgram != _programId) glUseProgram(oldProgram);
        
        return true;
    }
    
    bool ShaderOGL::setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
    {
        GLint oldProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);
        
        if (oldProgram != _programId) glUseProgram(_programId);
        glUniformMatrix4fv(index, static_cast<GLsizei>(matrices.size()), GL_FALSE, reinterpret_cast<const float*>(matrices.data()));
        
        if (oldProgram != _programId) glUseProgram(oldProgram);
        
        return true;
    }
}
