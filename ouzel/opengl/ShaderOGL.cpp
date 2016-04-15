// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Engine.h"
#include "RendererOGL.h"
#include "FileSystem.h"
#include "ShaderOGL.h"
#include "Utils.h"

namespace ouzel
{
    namespace video
    {
        ShaderOGL::ShaderOGL():
            Shader()
        {
        }

        ShaderOGL::~ShaderOGL()
        {
            destroy();
        }

        void ShaderOGL::destroy()
        {
            if (_programId)
            {
                glDeleteProgram(_programId);
                _programId = 0;
            }
            
            if (_vertexShaderId)
            {
                glDeleteShader(_vertexShaderId);
                _vertexShaderId = 0;
            }

            if (_pixelShaderId)
            {
                glDeleteShader(_pixelShaderId);
                _pixelShaderId = 0;
            }
        }

        bool ShaderOGL::initFromBuffers(const uint8_t* pixelShader,
                                        uint32_t pixelShaderSize,
                                        const uint8_t* vertexShader,
                                        uint32_t vertexShaderSize,
                                        uint32_t vertexAttributes,
                                        const std::string& pixelShaderFunction,
                                        const std::string& vertexShaderFunction)
        {
            if (!Shader::initFromBuffers(pixelShader, pixelShaderSize, vertexShader, vertexShaderSize, vertexAttributes, pixelShaderFunction, vertexShaderFunction))
            {
                return false;
            }

            destroy();

            GLboolean support;
            glGetBooleanv(GL_SHADER_COMPILER, &support);

            if (!support)
            {
                log("No shader compiler present");
                return false;
            }

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            _pixelShaderId = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(_pixelShaderId, 1, reinterpret_cast<const GLchar**>(&pixelShader), reinterpret_cast<const GLint*>(&pixelShaderSize));
            glCompileShader(_pixelShaderId);

            printShaderMessage(_pixelShaderId);

            GLint status;
            glGetShaderiv(_pixelShaderId, GL_COMPILE_STATUS, &status);
            if (status == GL_FALSE)
            {
                return false;
            }

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            _vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(_vertexShaderId, 1, reinterpret_cast<const GLchar**>(&vertexShader), reinterpret_cast<const GLint*>(&vertexShaderSize));
            glCompileShader(_vertexShaderId);

            printShaderMessage(_vertexShaderId);

            glGetShaderiv(_vertexShaderId, GL_COMPILE_STATUS, &status);
            if (status == GL_FALSE)
            {
                log("Failed to link shader");
                return false;
            }

            _programId = glCreateProgram();
            glAttachShader(_programId, _vertexShaderId);
            glAttachShader(_programId, _pixelShaderId);

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

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            RendererOGL::bindProgram(_programId);

            GLint texture0Location = glGetUniformLocation(_programId, "texture0");
            if (texture0Location != -1) glUniform1i(texture0Location, 0);

            GLint texture1Location = glGetUniformLocation(_programId, "texture1");
            if (texture1Location != -1) glUniform1i(texture1Location, 1);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            glDetachShader(_programId, _vertexShaderId);
            glDeleteShader(_vertexShaderId);
            _vertexShaderId = 0;

            glDetachShader(_programId, _pixelShaderId);
            glDeleteShader(_pixelShaderId);
            _pixelShaderId = 0;

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
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

        bool ShaderOGL::setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            Shader::setPixelShaderConstantInfo(constantInfo, alignment);

            _pixelShaderConstantLocations.reserve(constantInfo.size());

            for (const ConstantInfo& info : _pixelShaderConstantInfo)
            {
                GLint location = glGetUniformLocation(_programId, info.name.c_str());

                if (location == -1)
                {
                    RendererOGL::checkOpenGLErrors();
                    return false;
                }

                _pixelShaderConstantLocations.push_back(location);
            }

            return true;
        }

        bool ShaderOGL::setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            Shader::setVertexShaderConstantInfo(constantInfo, alignment);

            _vertexShaderConstantLocations.reserve(constantInfo.size());

            for (const ConstantInfo& info : _vertexShaderConstantInfo)
            {
                GLint location = glGetUniformLocation(_programId, info.name.c_str());

                if (location == -1)
                {
                    RendererOGL::checkOpenGLErrors();
                    return false;
                }

                _vertexShaderConstantLocations.push_back(location);
            }

            return true;
        }

        bool ShaderOGL::setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
        {
            if (index >= _pixelShaderConstantLocations.size()) return false;

            RendererOGL::bindProgram(_programId);

            GLint location = _pixelShaderConstantLocations[index];
            glUniform3fv(location, static_cast<GLsizei>(vectors.size()), reinterpret_cast<const float*>(vectors.data()));

            return true;
        }

        bool ShaderOGL::setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
        {
            if (index >= _pixelShaderConstantLocations.size()) return false;

            RendererOGL::bindProgram(_programId);

            GLint location = _pixelShaderConstantLocations[index];
            glUniform4fv(location, static_cast<GLsizei>(vectors.size()), reinterpret_cast<const float*>(vectors.data()));

            return true;
        }

        bool ShaderOGL::setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
        {
            if (index >= _pixelShaderConstantLocations.size()) return false;

            RendererOGL::bindProgram(_programId);

            GLint location = _pixelShaderConstantLocations[index];
            glUniformMatrix4fv(location, static_cast<GLsizei>(matrices.size()), GL_FALSE, reinterpret_cast<const float*>(matrices.data()));

            return true;
        }

        bool ShaderOGL::setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
        {
            if (index >= _vertexShaderConstantLocations.size()) return false;

            RendererOGL::bindProgram(_programId);

            GLint location = _vertexShaderConstantLocations[index];
            glUniform3fv(location, static_cast<GLsizei>(vectors.size()), reinterpret_cast<const float*>(vectors.data()));

            return true;
        }

        bool ShaderOGL::setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
        {
            if (index >= _vertexShaderConstantLocations.size()) return false;

            RendererOGL::bindProgram(_programId);

            GLint location = _vertexShaderConstantLocations[index];
            glUniform4fv(location, static_cast<GLsizei>(vectors.size()), reinterpret_cast<const float*>(vectors.data()));

            return true;
        }

        bool ShaderOGL::setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
        {
            if (index >= _vertexShaderConstantLocations.size()) return false;

            RendererOGL::bindProgram(_programId);

            GLint location = _vertexShaderConstantLocations[index];
            glUniformMatrix4fv(location, static_cast<GLsizei>(matrices.size()), GL_FALSE, reinterpret_cast<const float*>(matrices.data()));

            return true;
        }
    } // namespace video
} // namespace ouzel
