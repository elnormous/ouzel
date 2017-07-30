// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#include "ShaderInterfaceOGL.h"
#include "RendererOGL.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        ShaderInterfaceOGL::ShaderInterfaceOGL()
        {
        }

        ShaderInterfaceOGL::~ShaderInterfaceOGL()
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

        bool ShaderInterfaceOGL::init(const std::vector<uint8_t>& newPixelShader,
                                     const std::vector<uint8_t>& newVertexShader,
                                     const std::vector<VertexAttribute>& newVertexAttributes,
                                     const std::vector<Shader::ConstantInfo>& newPixelShaderConstantInfo,
                                     const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                                     uint32_t newPixelShaderDataAlignment,
                                     uint32_t newVertexShaderDataAlignment,
                                     const std::string& newPixelShaderFunction,
                                     const std::string& newVertexShaderFunction)
        {
            if (!ShaderInterface::init(newPixelShader,
                                      newVertexShader,
                                      newVertexAttributes,
                                      newPixelShaderConstantInfo,
                                      newVertexShaderConstantInfo,
                                      newPixelShaderDataAlignment,
                                      newVertexShaderDataAlignment,
                                      newPixelShaderFunction,
                                      newVertexShaderFunction))
            {
                return false;
            }

            if (!pixelShaderId)
            {
                pixelShaderId = glCreateShaderProc(GL_FRAGMENT_SHADER);

                const GLchar* pixelShaderBuffer = reinterpret_cast<const GLchar*>(pixelShaderData.data());
                GLint pixelShaderSize = static_cast<GLint>(pixelShaderData.size());

                glShaderSourceProc(pixelShaderId, 1, &pixelShaderBuffer, &pixelShaderSize);
                glCompileShaderProc(pixelShaderId);

                GLint status;
                glGetShaderivProc(pixelShaderId, GL_COMPILE_STATUS, &status);
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
                vertexShaderId = glCreateShaderProc(GL_VERTEX_SHADER);

                const GLchar* vertexShaderBuffer = reinterpret_cast<const GLchar*>(vertexShaderData.data());
                GLint vertexShaderSize = static_cast<GLint>(vertexShaderData.size());

                glShaderSourceProc(vertexShaderId, 1, &vertexShaderBuffer, &vertexShaderSize);
                glCompileShaderProc(vertexShaderId);

                GLint status;
                glGetShaderivProc(vertexShaderId, GL_COMPILE_STATUS, &status);
                if (status == GL_FALSE)
                {
                    Log(Log::Level::ERR) << "Failed to compile vertex shader";
                    printShaderMessage(vertexShaderId);
                    return false;
                }
            }

            if (!programId)
            {
                programId = glCreateProgramProc();

                glAttachShaderProc(programId, vertexShaderId);
                glAttachShaderProc(programId, pixelShaderId);

                GLuint index = 0;

                for (const VertexAttribute& vertexAttribute : vertexAttributes)
                {
                    std::string name;

                    switch (vertexAttribute.usage)
                    {
                        case VertexAttribute::Usage::BINORMAL:
                            name = "binormal" + std::to_string(vertexAttribute.index);
                            break;
                        case VertexAttribute::Usage::BLEND_INDICES:
                            name = "blendIndices" + std::to_string(vertexAttribute.index);
                            break;
                        case VertexAttribute::Usage::BLEND_WEIGHT:
                            name = "blendWeight" + std::to_string(vertexAttribute.index);
                            break;
                        case VertexAttribute::Usage::COLOR:
                            name = "color" + std::to_string(vertexAttribute.index);
                            break;
                        case VertexAttribute::Usage::NORMAL:
                            name = "normal" + std::to_string(vertexAttribute.index);
                            break;
                        case VertexAttribute::Usage::POSITION:
                            name = "position" + std::to_string(vertexAttribute.index);
                            break;
                        case VertexAttribute::Usage::POSITION_TRANSFORMED:
                            name = "positionT";
                            break;
                        case VertexAttribute::Usage::POINT_SIZE:
                            name = "pointSize" + std::to_string(vertexAttribute.index);
                            break;
                        case VertexAttribute::Usage::TANGENT:
                            name = "tangent" + std::to_string(vertexAttribute.index);
                            break;
                        case VertexAttribute::Usage::TEXTURE_COORDINATES:
                            name = "texCoord" + std::to_string(vertexAttribute.index);
                            break;
                        default:
                            Log(Log::Level::ERR) << "Invalid vertex attribute usage";
                            return false;
                    }

                    glBindAttribLocationProc(programId, index, name.c_str());
                    ++index;
                }

                glLinkProgramProc(programId);

                GLint status;
                glGetProgramivProc(programId, GL_LINK_STATUS, &status);
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

                glDetachShaderProc(programId, vertexShaderId);
                glDeleteShaderProc(vertexShaderId);
                vertexShaderId = 0;

                glDetachShaderProc(programId, pixelShaderId);
                glDeleteShaderProc(pixelShaderId);
                pixelShaderId = 0;

                if (RendererOGL::checkOpenGLError())
                {
                    return false;
                }

                RendererOGL::useProgram(programId);

                GLint texture0Location = glGetUniformLocationProc(programId, "texture0");
                if (texture0Location != -1) glUniform1iProc(texture0Location, 0);

                GLint texture1Location = glGetUniformLocationProc(programId, "texture1");
                if (texture1Location != -1) glUniform1iProc(texture1Location, 1);

                if (RendererOGL::checkOpenGLError())
                {
                    return false;
                }
            }

            if (!pixelShaderConstantInfo.empty())
            {
                pixelShaderConstantLocations.clear();
                pixelShaderConstantLocations.reserve(pixelShaderConstantInfo.size());

                for (const Shader::ConstantInfo& info : pixelShaderConstantInfo)
                {
                    GLint location = glGetUniformLocationProc(programId, info.name.c_str());

                    if (location == -1 || RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to get OpenGL uniform location";
                        return false;
                    }

                    pixelShaderConstantLocations.push_back({location, info.dataType});
                }
            }

            if (!vertexShaderConstantInfo.empty())
            {
                vertexShaderConstantLocations.clear();
                vertexShaderConstantLocations.reserve(vertexShaderConstantInfo.size());

                for (const Shader::ConstantInfo& info : vertexShaderConstantInfo)
                {
                    GLint location = glGetUniformLocationProc(programId, info.name.c_str());

                    if (location == -1 || RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to get OpenGL uniform location";
                        return false;
                    }

                    vertexShaderConstantLocations.push_back({location, info.dataType});
                }
            }

            return true;
        }

        void ShaderInterfaceOGL::printShaderMessage(GLuint shaderId)
        {
            GLint logLength = 0;
            glGetShaderivProc(shaderId, GL_INFO_LOG_LENGTH, &logLength);

            if (logLength > 0)
            {
                std::vector<char> logMessage(static_cast<size_t>(logLength));
                glGetShaderInfoLogProc(shaderId, logLength, nullptr, logMessage.data());

                Log(Log::Level::ERR) << "Shader compilation error: " << logMessage.data();
            }
        }

        void ShaderInterfaceOGL::printProgramMessage()
        {
            GLint logLength = 0;
            glGetProgramivProc(programId, GL_INFO_LOG_LENGTH, &logLength);

            if (logLength > 0)
            {
                std::vector<char> logMessage(static_cast<size_t>(logLength));
                glGetProgramInfoLogProc(programId, logLength, nullptr, logMessage.data());

                Log(Log::Level::ERR) << "Shader linking error: " << logMessage.data();
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
