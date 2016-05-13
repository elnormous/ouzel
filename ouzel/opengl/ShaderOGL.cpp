// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Engine.h"
#include "RendererOGL.h"
#include "FileSystem.h"
#include "ShaderOGL.h"
#include "Utils.h"

namespace ouzel
{
    namespace graphics
    {
        ShaderOGL::ShaderOGL():
            Shader()
        {
        }

        ShaderOGL::~ShaderOGL()
        {
            free();
        }

        void ShaderOGL::free()
        {
            if (programId)
            {
                glDeleteProgram(programId);
                programId = 0;
            }

            if (vertexShaderId)
            {
                glDeleteShader(vertexShaderId);
                vertexShaderId = 0;
            }

            if (pixelShaderId)
            {
                glDeleteShader(pixelShaderId);
                pixelShaderId = 0;
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

            free();

            pixelShaderId = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(pixelShaderId, 1, reinterpret_cast<const GLchar**>(&pixelShader), reinterpret_cast<const GLint*>(&pixelShaderSize));
            glCompileShader(pixelShaderId);

            GLint status;
            glGetShaderiv(pixelShaderId, GL_COMPILE_STATUS, &status);
            if (status == GL_FALSE)
            {
            printShaderMessage(pixelShaderId);
                return false;
            }

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShaderId, 1, reinterpret_cast<const GLchar**>(&vertexShader), reinterpret_cast<const GLint*>(&vertexShaderSize));
            glCompileShader(vertexShaderId);

            glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &status);
            if (status == GL_FALSE)
            {
                printShaderMessage(vertexShaderId);
                log("Failed to link shader");
                return false;
            }

            programId = glCreateProgram();
            glAttachShader(programId, vertexShaderId);
            glAttachShader(programId, pixelShaderId);

            GLuint index = 0;

            if (vertexAttributes & VERTEX_POSITION)
            {
                glBindAttribLocation(programId, index, "in_Position");
                ++index;
            }

            if (vertexAttributes & VERTEX_COLOR)
            {
                glBindAttribLocation(programId, index, "in_Color");
                ++index;
            }

            if (vertexAttributes & VERTEX_NORMAL)
            {
                glBindAttribLocation(programId, index, "in_Normal");
                ++index;
            }

            if (vertexAttributes & VERTEX_TEXCOORD0)
            {
                glBindAttribLocation(programId, index, "in_TexCoord0");
                ++index;
            }

            if (vertexAttributes & VERTEX_TEXCOORD1)
            {
                glBindAttribLocation(programId, index, "in_TexCoord1");
                ++index;
            }

            glLinkProgram(programId);

            glGetProgramiv(programId, GL_LINK_STATUS, &status);
            if (status == GL_FALSE)
            {
                printProgramMessage();
                return false;
            }

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            RendererOGL::bindProgram(programId);

            GLint texture0Location = glGetUniformLocation(programId, "texture0");
            if (texture0Location != -1) glUniform1i(texture0Location, 0);

            GLint texture1Location = glGetUniformLocation(programId, "texture1");
            if (texture1Location != -1) glUniform1i(texture1Location, 1);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            glDetachShader(programId, vertexShaderId);
            glDeleteShader(vertexShaderId);
            vertexShaderId = 0;

            glDetachShader(programId, pixelShaderId);
            glDeleteShader(pixelShaderId);
            pixelShaderId = 0;

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
                std::vector<char> logMessage(logLength);
                glGetShaderInfoLog(shaderId, logLength, nullptr, logMessage.data());

                log("Shader compilation error: %s", logMessage.data());
            }
        }

        void ShaderOGL::printProgramMessage()
        {
            GLint logLength = 0;
            glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);

            if (logLength > 0)
            {
                std::vector<char> logMessage(logLength);
                glGetProgramInfoLog(programId, logLength, nullptr, logMessage.data());

                log("Shader linking error: %s", logMessage.data());
            }
        }

        bool ShaderOGL::setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            Shader::setPixelShaderConstantInfo(constantInfo, alignment);

            pixelShaderConstantLocations.reserve(constantInfo.size());

            for (const ConstantInfo& info : pixelShaderConstantInfo)
            {
                GLint location = glGetUniformLocation(programId, info.name.c_str());

                if (location == -1)
                {
                    RendererOGL::checkOpenGLErrors();
                    return false;
                }

                pixelShaderConstantLocations.push_back(location);
            }

            return true;
        }

        bool ShaderOGL::setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            Shader::setVertexShaderConstantInfo(constantInfo, alignment);

            vertexShaderConstantLocations.reserve(constantInfo.size());

            for (const ConstantInfo& info : vertexShaderConstantInfo)
            {
                GLint location = glGetUniformLocation(programId, info.name.c_str());

                if (location == -1)
                {
                    RendererOGL::checkOpenGLErrors();
                    return false;
                }

                vertexShaderConstantLocations.push_back(location);
            }

            return true;
        }

        bool ShaderOGL::setPixelShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value)
        {
            if (index >= pixelShaderConstantLocations.size()) return false;

            RendererOGL::bindProgram(programId);

            GLint location = pixelShaderConstantLocations[index];

            uint32_t components = size / 4;

            switch (components)
            {
                case 1:
                    glUniform1fv(location, static_cast<GLsizei>(count), value);
                    break;
                case 2:
                    glUniform2fv(location, static_cast<GLsizei>(count), value);
                    break;
                case 3:
                    glUniform3fv(location, static_cast<GLsizei>(count), value);
                    break;
                case 4:
                    glUniform4fv(location, static_cast<GLsizei>(count), value);
                    break;
                case 9:
                    glUniformMatrix3fv(location, static_cast<GLsizei>(count), GL_FALSE, value);
                    break;
                case 16:
                    glUniformMatrix4fv(location, static_cast<GLsizei>(count), GL_FALSE, value);
                    break;
                default:
                    log("Unsupported uniform size");
                    return false;
            }

            return true;
        }

        bool ShaderOGL::setVertexShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value)
        {
            if (index >= vertexShaderConstantLocations.size()) return false;

            RendererOGL::bindProgram(programId);

            GLint location = vertexShaderConstantLocations[index];

            uint32_t components = size / 4;

            switch (components)
            {
                case 1:
                    glUniform1fv(location, static_cast<GLsizei>(count), value);
                    break;
                case 2:
                    glUniform2fv(location, static_cast<GLsizei>(count), value);
                    break;
                case 3:
                    glUniform3fv(location, static_cast<GLsizei>(count), value);
                    break;
                case 4:
                    glUniform4fv(location, static_cast<GLsizei>(count), value);
                    break;
                case 9:
                    glUniformMatrix3fv(location, static_cast<GLsizei>(count), GL_FALSE, value);
                    break;
                case 16:
                    glUniformMatrix4fv(location, static_cast<GLsizei>(count), GL_FALSE, value);
                    break;
                default:
                    log("Unsupported uniform size");
                    return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
