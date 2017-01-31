// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ShaderOGL.h"
#include "core/Engine.h"
#include "RendererOGL.h"
#include "files/FileSystem.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        ShaderOGL::ShaderOGL()
        {
        }

        ShaderOGL::~ShaderOGL()
        {
            if (programId)
            {
                RendererOGL::deleteProgram(programId);
            }

            if (vertexShaderId)
            {
                RendererOGL::deleteShader(vertexShaderId);
            }

            if (pixelShaderId)
            {
                RendererOGL::deleteShader(pixelShaderId);
            }
        }

        void ShaderOGL::printShaderMessage(GLuint shaderId)
        {
            GLint logLength = 0;
            glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);

            if (logLength > 0)
            {
                std::vector<char> logMessage(static_cast<size_t>(logLength));
                glGetShaderInfoLog(shaderId, logLength, nullptr, logMessage.data());

                Log(Log::Level::ERR) << "Shader compilation error: " << logMessage.data();
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

                Log(Log::Level::ERR) << "Shader linking error: " << logMessage.data();
            }
        }

        bool ShaderOGL::upload()
        {
            if (!ShaderResource::upload())
            {
                return false;
            }

            if (data.dirty)
            {
                if (!pixelShaderId)
                {
                    pixelShaderId = glCreateShader(GL_FRAGMENT_SHADER);

                    const GLchar* pixelShaderBuffer = reinterpret_cast<const GLchar*>(data.pixelShaderData.data());
                    GLint pixelShaderSize = static_cast<GLint>(data.pixelShaderData.size());

                    glShaderSource(pixelShaderId, 1, &pixelShaderBuffer, &pixelShaderSize);
                    glCompileShader(pixelShaderId);

                    GLint status;
                    glGetShaderiv(pixelShaderId, GL_COMPILE_STATUS, &status);
                    if (status == GL_FALSE)
                    {
                        Log(Log::Level::ERR) << "Failed to compile pixel shader";
                        printShaderMessage(pixelShaderId);
                        return false;
                    }

                    if (RendererOGL::checkOpenGLError())
                    {
                        return false;
                    }
                }

                if (!vertexShaderId)
                {
                    vertexShaderId = glCreateShader(GL_VERTEX_SHADER);

                    const GLchar* vertexShaderBuffer = reinterpret_cast<const GLchar*>(data.vertexShaderData.data());
                    GLint vertexShaderSize = static_cast<GLint>(data.vertexShaderData.size());

                    glShaderSource(vertexShaderId, 1, &vertexShaderBuffer, &vertexShaderSize);
                    glCompileShader(vertexShaderId);

                    GLint status;
                    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &status);
                    if (status == GL_FALSE)
                    {
                        Log(Log::Level::ERR) << "Failed to compile vertex shader";
                        printShaderMessage(vertexShaderId);
                        return false;
                    }
                }

                if (!programId)
                {
                    programId = glCreateProgram();

                    glAttachShader(programId, vertexShaderId);
                    glAttachShader(programId, pixelShaderId);

                    GLuint index = 0;

                    for (const VertexAttribute& vertexAttribute : data.vertexAttributes)
                    {
                        glBindAttribLocation(programId, index, vertexAttribute.name.c_str());
                        ++index;
                    }

                    glLinkProgram(programId);

                    GLint status;
                    glGetProgramiv(programId, GL_LINK_STATUS, &status);
                    if (status == GL_FALSE)
                    {
                        Log(Log::Level::ERR) << "Failed to link shader";
                        printProgramMessage();
                        return false;
                    }

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

                    RendererOGL::useProgram(programId);

                    GLint texture0Location = glGetUniformLocation(programId, "texture0");
                    if (texture0Location != -1) glUniform1i(texture0Location, 0);

                    GLint texture1Location = glGetUniformLocation(programId, "texture1");
                    if (texture1Location != -1) glUniform1i(texture1Location, 1);

                    if (RendererOGL::checkOpenGLError())
                    {
                        return false;
                    }
                }

                if (!data.pixelShaderConstantInfo.empty())
                {
                    pixelShaderConstantLocations.clear();
                    pixelShaderConstantLocations.reserve(data.pixelShaderConstantInfo.size());

                    for (const Shader::ConstantInfo& info : data.pixelShaderConstantInfo)
                    {
                        GLint location = glGetUniformLocation(programId, info.name.c_str());

                        if (location == -1 || RendererOGL::checkOpenGLError())
                        {
                            Log(Log::Level::ERR) << "Failed to get OpenGL uniform location";
                            return false;
                        }

                        pixelShaderConstantLocations.push_back({location, info.dataType});
                    }
                }

                if (!data.vertexShaderConstantInfo.empty())
                {
                    vertexShaderConstantLocations.clear();
                    vertexShaderConstantLocations.reserve(data.vertexShaderConstantInfo.size());

                    for (const Shader::ConstantInfo& info : data.vertexShaderConstantInfo)
                    {
                        GLint location = glGetUniformLocation(programId, info.name.c_str());

                        if (location == -1 || RendererOGL::checkOpenGLError())
                        {
                            Log(Log::Level::ERR) << "Failed to get OpenGL uniform location";
                            return false;
                        }

                        vertexShaderConstantLocations.push_back({location, info.dataType});
                    }
                }

                data.dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
