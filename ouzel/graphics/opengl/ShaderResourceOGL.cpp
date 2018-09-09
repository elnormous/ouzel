// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "ShaderResourceOGL.hpp"
#include "RenderDeviceOGL.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace graphics
    {
        ShaderResourceOGL::ShaderResourceOGL(RenderDeviceOGL& renderDeviceOGL):
            RenderResourceOGL(renderDeviceOGL)
        {
        }

        ShaderResourceOGL::~ShaderResourceOGL()
        {
            RenderDeviceOGL& renderDeviceOGL = static_cast<RenderDeviceOGL&>(renderDevice);
            if (programId) renderDeviceOGL.deleteProgram(programId);
            if (vertexShaderId) glDeleteShaderProc(vertexShaderId);
            if (fragmentShaderId) glDeleteShaderProc(fragmentShaderId);
        }

        void ShaderResourceOGL::init(const std::vector<uint8_t>& newFragmentShader,
                                     const std::vector<uint8_t>& newVertexShader,
                                     const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                                     const std::vector<Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                                     const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                                     uint32_t newFragmentShaderDataAlignment,
                                     uint32_t newVertexShaderDataAlignment,
                                     const std::string& newFragmentShaderFunction,
                                     const std::string& newVertexShaderFunction)
        {
            fragmentShaderData = newFragmentShader;
            vertexShaderData = newVertexShader;
            vertexAttributes = newVertexAttributes;
            fragmentShaderConstantInfo = newFragmentShaderConstantInfo;
            vertexShaderConstantInfo = newVertexShaderConstantInfo;

            if (newFragmentShaderDataAlignment)
                fragmentShaderAlignment = newFragmentShaderDataAlignment;
            else
            {
                fragmentShaderAlignment = 0;

                for (const Shader::ConstantInfo& info : newFragmentShaderConstantInfo)
                    fragmentShaderAlignment += info.size;
            }

            if (newVertexShaderDataAlignment)
                vertexShaderAlignment = newVertexShaderDataAlignment;
            else
            {
                vertexShaderAlignment = 0;

                for (const Shader::ConstantInfo& info : newVertexShaderConstantInfo)
                    vertexShaderAlignment += info.size;
            }

            fragmentShaderFunction = newFragmentShaderFunction;
            vertexShaderFunction = newVertexShaderFunction;

            if (programId)
            {
                RenderDeviceOGL& renderDeviceOGL = static_cast<RenderDeviceOGL&>(renderDevice);
                renderDeviceOGL.deleteProgram(programId);
                programId = 0;
            }

            if (vertexShaderId)
            {
                glDeleteShaderProc(vertexShaderId);
                vertexShaderId = 0;
            }

            if (fragmentShaderId)
            {
                glDeleteShaderProc(fragmentShaderId);
                fragmentShaderId = 0;
            }

            compileShader();
        }

        void ShaderResourceOGL::reload()
        {
            fragmentShaderId = 0;
            vertexShaderId = 0;
            programId = 0;

            compileShader();
        }

        std::string ShaderResourceOGL::getShaderMessage(GLuint shaderId)
        {
            GLint logLength = 0;
            glGetShaderivProc(shaderId, GL_INFO_LOG_LENGTH, &logLength);

            if (logLength > 0)
            {
                std::vector<char> logMessage(static_cast<size_t>(logLength));
                glGetShaderInfoLogProc(shaderId, logLength, nullptr, logMessage.data());

                return std::string(logMessage.data());
            }

            return std::string();
        }

        std::string ShaderResourceOGL::getProgramMessage()
        {
            GLint logLength = 0;
            glGetProgramivProc(programId, GL_INFO_LOG_LENGTH, &logLength);

            if (logLength > 0)
            {
                std::vector<char> logMessage(static_cast<size_t>(logLength));
                glGetProgramInfoLogProc(programId, logLength, nullptr, logMessage.data());

                return std::string("Shader linking error: " + std::string(logMessage.data()));
            }

            return std::string();
        }

        void ShaderResourceOGL::compileShader()
        {
            fragmentShaderId = glCreateShaderProc(GL_FRAGMENT_SHADER);

            const GLchar* fragmentShaderBuffer = reinterpret_cast<const GLchar*>(fragmentShaderData.data());
            GLint fragmentShaderSize = static_cast<GLint>(fragmentShaderData.size());

            glShaderSourceProc(fragmentShaderId, 1, &fragmentShaderBuffer, &fragmentShaderSize);
            glCompileShaderProc(fragmentShaderId);

            GLint status;
            glGetShaderivProc(fragmentShaderId, GL_COMPILE_STATUS, &status);
            if (status == GL_FALSE)
                throw DataError("Failed to compile pixel shader, error: " + getShaderMessage(fragmentShaderId));

            GLenum error;

            if ((error = glGetError()) != GL_NO_ERROR)
                throw DataError("Failed to get shader compile status, error: " + std::to_string(error));

            vertexShaderId = glCreateShaderProc(GL_VERTEX_SHADER);

            const GLchar* vertexShaderBuffer = reinterpret_cast<const GLchar*>(vertexShaderData.data());
            GLint vertexShaderSize = static_cast<GLint>(vertexShaderData.size());

            glShaderSourceProc(vertexShaderId, 1, &vertexShaderBuffer, &vertexShaderSize);
            glCompileShaderProc(vertexShaderId);

            glGetShaderivProc(vertexShaderId, GL_COMPILE_STATUS, &status);
            if (status == GL_FALSE)
                throw DataError("Failed to compile vertex shader, error: " + getShaderMessage(vertexShaderId));

            programId = glCreateProgramProc();

            glAttachShaderProc(programId, vertexShaderId);
            glAttachShaderProc(programId, fragmentShaderId);

            GLuint index = 0;

            for (const Vertex::Attribute& vertexAttribute : Vertex::ATTRIBUTES)
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
                            throw DataError("Invalid vertex attribute usage");
                    }

                    glBindAttribLocationProc(programId, index, name);
                    ++index;
                }
            }

            glLinkProgramProc(programId);

            glGetProgramivProc(programId, GL_LINK_STATUS, &status);
            if (status == GL_FALSE)
                throw DataError("Failed to link shader" + getProgramMessage());

            if ((error = glGetError()) != GL_NO_ERROR)
                throw DataError("Failed to get shader link status, error: " + std::to_string(error));

            glDetachShaderProc(programId, vertexShaderId);
            glDeleteShaderProc(vertexShaderId);
            vertexShaderId = 0;

            glDetachShaderProc(programId, fragmentShaderId);
            glDeleteShaderProc(fragmentShaderId);
            fragmentShaderId = 0;

            if ((error = glGetError()) != GL_NO_ERROR)
                throw DataError("Failed to detach shader, error: " + std::to_string(error));

            RenderDeviceOGL& renderDeviceOGL = static_cast<RenderDeviceOGL&>(renderDevice);
            renderDeviceOGL.useProgram(programId);

            GLint texture0Location = glGetUniformLocationProc(programId, "texture0");
            if (texture0Location != -1) glUniform1iProc(texture0Location, 0);

            GLint texture1Location = glGetUniformLocationProc(programId, "texture1");
            if (texture1Location != -1) glUniform1iProc(texture1Location, 1);

            if ((error = glGetError()) != GL_NO_ERROR)
                throw DataError("Failed to get uniform location, error: " + std::to_string(error));

            if (!fragmentShaderConstantInfo.empty())
            {
                fragmentShaderConstantLocations.clear();
                fragmentShaderConstantLocations.reserve(fragmentShaderConstantInfo.size());

                for (const Shader::ConstantInfo& info : fragmentShaderConstantInfo)
                {
                    GLint location = glGetUniformLocationProc(programId, info.name.c_str());

                    if ((error = glGetError()) != GL_NO_ERROR)
                        throw DataError("Failed to get OpenGL uniform location, error: " + std::to_string(error));

                    if (location == -1)
                        throw DataError("Failed to get OpenGL uniform location");

                    fragmentShaderConstantLocations.push_back({location, info.dataType});
                }
            }

            if (!vertexShaderConstantInfo.empty())
            {
                vertexShaderConstantLocations.clear();
                vertexShaderConstantLocations.reserve(vertexShaderConstantInfo.size());

                for (const Shader::ConstantInfo& info : vertexShaderConstantInfo)
                {
                    GLint location = glGetUniformLocationProc(programId, info.name.c_str());

                    if ((error = glGetError()) != GL_NO_ERROR)
                        throw DataError("Failed to get OpenGL uniform location, error: " + std::to_string(error));

                    if (location == -1)
                        throw DataError("Failed to get OpenGL uniform location");

                    vertexShaderConstantLocations.push_back({location, info.dataType});
                }
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
