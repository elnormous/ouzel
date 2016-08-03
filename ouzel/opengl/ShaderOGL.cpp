// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ShaderOGL.h"
#include "core/Engine.h"
#include "RendererOGL.h"
#include "files/FileSystem.h"
#include "utils/Utils.h"

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
            std::shared_ptr<RendererOGL> rendererOGL = std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer());

            if (programId)
            {
                rendererOGL->deleteResource(programId, RendererOGL::ResourceType::Program);
            }

            if (vertexShaderId)
            {
                rendererOGL->deleteResource(vertexShaderId, RendererOGL::ResourceType::Shader);
            }

            if (pixelShaderId)
            {
                rendererOGL->deleteResource(pixelShaderId, RendererOGL::ResourceType::Shader);
            }
        }

        void ShaderOGL::free()
        {
            Shader::free();

            std::shared_ptr<RendererOGL> rendererOGL = std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer());

            if (programId)
            {
                rendererOGL->deleteResource(programId, RendererOGL::ResourceType::Program);
                programId = 0;
            }

            if (vertexShaderId)
            {
                rendererOGL->deleteResource(vertexShaderId, RendererOGL::ResourceType::Shader);
                vertexShaderId = 0;
            }

            if (pixelShaderId)
            {
                rendererOGL->deleteResource(pixelShaderId, RendererOGL::ResourceType::Shader);
                pixelShaderId = 0;
            }
        }

        bool ShaderOGL::initFromBuffers(const std::vector<uint8_t>& newPixelShader,
                                        const std::vector<uint8_t>& newVertexShader,
                                        uint32_t newVertexAttributes,
                                        const std::string& pixelShaderFunction,
                                        const std::string& vertexShaderFunction)
        {
            if (!Shader::initFromBuffers(newPixelShader, newVertexShader, newVertexAttributes, pixelShaderFunction, vertexShaderFunction))
            {
                return false;
            }

            free();

            pixelShaderId = glCreateShader(GL_FRAGMENT_SHADER);

            const GLchar* pixelShaderBuffer = reinterpret_cast<const GLchar*>(newPixelShader.data());
            GLint pixelShaderSize = static_cast<GLint>(newPixelShader.size());

            glShaderSource(pixelShaderId, 1, &pixelShaderBuffer, &pixelShaderSize);
            glCompileShader(pixelShaderId);

            GLint status;
            glGetShaderiv(pixelShaderId, GL_COMPILE_STATUS, &status);
            if (status == GL_FALSE)
            {
                log("Failed to compile pixel shader");
                printShaderMessage(pixelShaderId);
                return false;
            }

            if (RendererOGL::checkOpenGLError())
            {
                return false;
            }

            vertexShaderId = glCreateShader(GL_VERTEX_SHADER);

            const GLchar* vertexShaderBuffer = reinterpret_cast<const GLchar*>(newVertexShader.data());
            GLint vertexShaderSize = static_cast<GLint>(newVertexShader.size());

            glShaderSource(vertexShaderId, 1, &vertexShaderBuffer, &vertexShaderSize);
            glCompileShader(vertexShaderId);

            glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &status);
            if (status == GL_FALSE)
            {
                log("Failed to compile vertex shader");
                printShaderMessage(vertexShaderId);
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
                log("Failed to link shader");
                printProgramMessage();
                return false;
            }

            if (RendererOGL::checkOpenGLError())
            {
                return false;
            }

            RendererOGL::bindProgram(programId);

            GLint texture0Location = glGetUniformLocation(programId, "texture0");
            if (texture0Location != -1) glUniform1i(texture0Location, 0);

            GLint texture1Location = glGetUniformLocation(programId, "texture1");
            if (texture1Location != -1) glUniform1i(texture1Location, 1);

            if (RendererOGL::checkOpenGLError())
            {
                return false;
            }

            glDetachShader(programId, vertexShaderId);
            glDeleteShader(vertexShaderId);
            vertexShaderId = 0;

            glDetachShader(programId, pixelShaderId);
            glDeleteShader(pixelShaderId);
            pixelShaderId = 0;

            if (RendererOGL::checkOpenGLError())
            {
                return false;
            }

            ready = true;

            return true;
        }

        void ShaderOGL::printShaderMessage(GLuint shaderId)
        {
            GLint logLength = 0;
            glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);

            if (logLength > 0)
            {
                std::vector<char> logMessage(static_cast<size_t>(logLength));
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
                std::vector<char> logMessage(static_cast<size_t>(logLength));
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

                if (location == -1 || RendererOGL::checkOpenGLError())
                {
                    log("Failed to get OpenGL uniform location");
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

                if (location == -1 || RendererOGL::checkOpenGLError())
                {
                    log("Failed to get OpenGL uniform location");
                    return false;
                }

                vertexShaderConstantLocations.push_back(location);
            }

            return true;
        }

        bool ShaderOGL::update()
        {
            if (pixelShaderDirty || vertexShaderDirty)
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                
                if (pixelShaderDirty)
                {
                    pixelShaderDirty = false;
                }

                if (vertexShaderDirty)
                {
                    vertexShaderDirty = false;
                }
            }
            
            return true;
        }
    } // namespace graphics
} // namespace ouzel
