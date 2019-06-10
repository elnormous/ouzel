// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGLShader.hpp"
#include "OGLRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            Shader::Shader(RenderDevice& renderDevice,
                           const std::vector<uint8_t>& newFragmentShader,
                           const std::vector<uint8_t>& newVertexShader,
                           const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                           const std::vector<graphics::Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                           const std::vector<graphics::Shader::ConstantInfo>& newVertexShaderConstantInfo,
                           uint32_t,
                           uint32_t,
                           const std::string&,
                           const std::string&):
                RenderResource(renderDevice),
                fragmentShaderData(newFragmentShader),
                vertexShaderData(newVertexShader),
                vertexAttributes(newVertexAttributes),
                fragmentShaderConstantInfo(newFragmentShaderConstantInfo),
                vertexShaderConstantInfo(newVertexShaderConstantInfo)
            {
                compileShader();
            }

            Shader::~Shader()
            {
                if (programId) renderDevice.deleteProgram(programId);
                if (vertexShaderId) renderDevice.glDeleteShaderProc(vertexShaderId);
                if (fragmentShaderId) renderDevice.glDeleteShaderProc(fragmentShaderId);
            }

            void Shader::reload()
            {
                fragmentShaderId = 0;
                vertexShaderId = 0;
                programId = 0;

                compileShader();
            }

            std::string Shader::getShaderMessage(GLuint shaderId) const
            {
                GLint logLength = 0;
                renderDevice.glGetShaderivProc(shaderId, GL_INFO_LOG_LENGTH, &logLength);

                if (logLength > 0)
                {
                    std::vector<char> logMessage(static_cast<size_t>(logLength));
                    renderDevice.glGetShaderInfoLogProc(shaderId, logLength, nullptr, logMessage.data());

                    return std::string(logMessage.data());
                }

                return std::string();
            }

            std::string Shader::getProgramMessage() const
            {
                GLint logLength = 0;
                renderDevice.glGetProgramivProc(programId, GL_INFO_LOG_LENGTH, &logLength);

                if (logLength > 0)
                {
                    std::vector<char> logMessage(static_cast<size_t>(logLength));
                    renderDevice.glGetProgramInfoLogProc(programId, logLength, nullptr, logMessage.data());

                    return std::string("Shader linking error: " + std::string(logMessage.data()));
                }

                return std::string();
            }

            void Shader::compileShader()
            {
                fragmentShaderId = renderDevice.glCreateShaderProc(GL_FRAGMENT_SHADER);

                const GLchar* fragmentShaderBuffer = reinterpret_cast<const GLchar*>(fragmentShaderData.data());
                GLint fragmentShaderSize = static_cast<GLint>(fragmentShaderData.size());

                renderDevice.glShaderSourceProc(fragmentShaderId, 1, &fragmentShaderBuffer, &fragmentShaderSize);
                renderDevice.glCompileShaderProc(fragmentShaderId);

                GLint status;
                renderDevice.glGetShaderivProc(fragmentShaderId, GL_COMPILE_STATUS, &status);
                if (status == GL_FALSE)
                    throw std::runtime_error("Failed to compile pixel shader, error: " + getShaderMessage(fragmentShaderId));

                GLenum error;

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to get shader compile status");

                vertexShaderId = renderDevice.glCreateShaderProc(GL_VERTEX_SHADER);

                const GLchar* vertexShaderBuffer = reinterpret_cast<const GLchar*>(vertexShaderData.data());
                GLint vertexShaderSize = static_cast<GLint>(vertexShaderData.size());

                renderDevice.glShaderSourceProc(vertexShaderId, 1, &vertexShaderBuffer, &vertexShaderSize);
                renderDevice.glCompileShaderProc(vertexShaderId);

                renderDevice.glGetShaderivProc(vertexShaderId, GL_COMPILE_STATUS, &status);
                if (status == GL_FALSE)
                    throw std::runtime_error("Failed to compile vertex shader, error: " + getShaderMessage(vertexShaderId));

                programId = renderDevice.glCreateProgramProc();

                renderDevice.glAttachShaderProc(programId, vertexShaderId);
                renderDevice.glAttachShaderProc(programId, fragmentShaderId);

                GLuint index = 0;

                for (const Vertex::Attribute& vertexAttribute : RenderDevice::VERTEX_ATTRIBUTES)
                {
                    if (vertexAttributes.find(vertexAttribute.usage) != vertexAttributes.end())
                    {
                        const GLchar* name;

                        switch (vertexAttribute.usage)
                        {
                            case Vertex::Attribute::Usage::BINORMAL:
                                name = "binormal0";
                                break;
                            case Vertex::Attribute::Usage::BLEND_INDICES:
                                name = "blendIndices0";
                                break;
                            case Vertex::Attribute::Usage::BLEND_WEIGHT:
                                name = "blendWeight0";
                                break;
                            case Vertex::Attribute::Usage::COLOR:
                                name = "color0";
                                break;
                            case Vertex::Attribute::Usage::NORMAL:
                                name = "normal0";
                                break;
                            case Vertex::Attribute::Usage::POSITION:
                                name = "position0";
                                break;
                            case Vertex::Attribute::Usage::POSITION_TRANSFORMED:
                                name = "positionT0";
                                break;
                            case Vertex::Attribute::Usage::POINT_SIZE:
                                name = "pointSize0";
                                break;
                            case Vertex::Attribute::Usage::TANGENT:
                                name = "tangent0";
                                break;
                            case Vertex::Attribute::Usage::TEXTURE_COORDINATES0:
                                name = "texCoord0";
                                break;
                            case Vertex::Attribute::Usage::TEXTURE_COORDINATES1:
                                name = "texCoord1";
                                break;
                            default:
                                throw std::runtime_error("Invalid vertex attribute usage");
                        }

                        renderDevice.glBindAttribLocationProc(programId, index, name);
                        ++index;
                    }
                }

                renderDevice.glLinkProgramProc(programId);

                renderDevice.glGetProgramivProc(programId, GL_LINK_STATUS, &status);
                if (status == GL_FALSE)
                    throw std::runtime_error("Failed to link shader" + getProgramMessage());

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to get shader link status");

                renderDevice.glDetachShaderProc(programId, vertexShaderId);
                renderDevice.glDeleteShaderProc(vertexShaderId);
                vertexShaderId = 0;

                renderDevice.glDetachShaderProc(programId, fragmentShaderId);
                renderDevice.glDeleteShaderProc(fragmentShaderId);
                fragmentShaderId = 0;

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to detach shader");

                renderDevice.useProgram(programId);

                GLint texture0Location = renderDevice.glGetUniformLocationProc(programId, "texture0");
                if (texture0Location != -1) renderDevice.glUniform1iProc(texture0Location, 0);

                GLint texture1Location = renderDevice.glGetUniformLocationProc(programId, "texture1");
                if (texture1Location != -1) renderDevice.glUniform1iProc(texture1Location, 1);

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to get uniform location");

                if (!fragmentShaderConstantInfo.empty())
                {
                    fragmentShaderConstantLocations.clear();
                    fragmentShaderConstantLocations.reserve(fragmentShaderConstantInfo.size());

                    for (const graphics::Shader::ConstantInfo& info : fragmentShaderConstantInfo)
                    {
                        GLint location = renderDevice.glGetUniformLocationProc(programId, info.name.c_str());

                        if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to get OpenGL uniform location");

                        if (location == -1)
                            throw std::runtime_error("Failed to get OpenGL uniform location");

                        fragmentShaderConstantLocations.push_back({location, info.dataType});
                    }
                }

                if (!vertexShaderConstantInfo.empty())
                {
                    vertexShaderConstantLocations.clear();
                    vertexShaderConstantLocations.reserve(vertexShaderConstantInfo.size());

                    for (const graphics::Shader::ConstantInfo& info : vertexShaderConstantInfo)
                    {
                        GLint location = renderDevice.glGetUniformLocationProc(programId, info.name.c_str());

                        if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to get OpenGL uniform location");

                        if (location == -1)
                            throw std::runtime_error("Failed to get OpenGL uniform location");

                        vertexShaderConstantLocations.push_back({location, info.dataType});
                    }
                }
            }
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
