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
            dirty(false)
        {
        }

        ShaderOGL::~ShaderOGL()
        {
            if (programId)
            {
                RendererOGL::deleteResource(programId, RendererOGL::ResourceType::Program);
            }

            if (vertexShaderId)
            {
                RendererOGL::deleteResource(vertexShaderId, RendererOGL::ResourceType::Shader);
            }

            if (pixelShaderId)
            {
                RendererOGL::deleteResource(pixelShaderId, RendererOGL::ResourceType::Shader);
            }
        }

        void ShaderOGL::free()
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            Shader::free();

            pixelShaderData.clear();
            vertexShaderData.clear();

            if (programId)
            {
                RendererOGL::deleteResource(programId, RendererOGL::ResourceType::Program);
                programId = 0;
            }

            if (vertexShaderId)
            {
                RendererOGL::deleteResource(vertexShaderId, RendererOGL::ResourceType::Shader);
                vertexShaderId = 0;
            }

            if (pixelShaderId)
            {
                RendererOGL::deleteResource(pixelShaderId, RendererOGL::ResourceType::Shader);
                pixelShaderId = 0;
            }
        }

        bool ShaderOGL::initFromBuffers(const std::vector<uint8_t>& newPixelShader,
                                        const std::vector<uint8_t>& newVertexShader,
                                        uint32_t newVertexAttributes,
                                        const std::string& pixelShaderFunction,
                                        const std::string& vertexShaderFunction)
        {
            free();

            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Shader::initFromBuffers(newPixelShader, newVertexShader, newVertexAttributes, pixelShaderFunction, vertexShaderFunction))
            {
                return false;
            }

            pixelShaderData = newPixelShader;
            vertexShaderData = newVertexShader;

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

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
            std::lock_guard<std::mutex> lock(dataMutex);

            Shader::setPixelShaderConstantInfo(constantInfo, alignment);

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool ShaderOGL::setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            Shader::setVertexShaderConstantInfo(constantInfo, alignment);

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool ShaderOGL::update()
        {
            if (dirty)
            {
                std::lock_guard<std::mutex> lock(dataMutex);

                if (!pixelShaderId)
                {
                    pixelShaderId = glCreateShader(GL_FRAGMENT_SHADER);
                }

                const GLchar* pixelShaderBuffer = reinterpret_cast<const GLchar*>(pixelShaderData.data());
                GLint pixelShaderSize = static_cast<GLint>(pixelShaderData.size());

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

                if (!vertexShaderId)
                {
                    vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
                }

                const GLchar* vertexShaderBuffer = reinterpret_cast<const GLchar*>(vertexShaderData.data());
                GLint vertexShaderSize = static_cast<GLint>(vertexShaderData.size());

                glShaderSource(vertexShaderId, 1, &vertexShaderBuffer, &vertexShaderSize);
                glCompileShader(vertexShaderId);

                glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &status);
                if (status == GL_FALSE)
                {
                    log("Failed to compile vertex shader");
                    printShaderMessage(vertexShaderId);
                    return false;
                }

                if (!programId)
                {
                    programId = glCreateProgram();
                }

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

                pixelShaderConstantLocations.clear();
                pixelShaderConstantLocations.reserve(pixelShaderConstantInfo.size());

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

                vertexShaderConstantLocations.clear();
                vertexShaderConstantLocations.reserve(vertexShaderConstantInfo.size());

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

                ready = true;
                dirty = false;
            }
            
            return true;
        }
    } // namespace graphics
} // namespace ouzel
