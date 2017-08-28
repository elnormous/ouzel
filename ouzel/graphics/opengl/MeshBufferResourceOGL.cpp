// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#include "MeshBufferResourceOGL.hpp"
#include "RenderDeviceOGL.hpp"
#include "BufferResourceOGL.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        static GLenum getVertexFormat(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::BYTE:
                case DataType::BYTE_VECTOR2:
                case DataType::BYTE_VECTOR3:
                case DataType::BYTE_VECTOR4:
                    return GL_BYTE;

                case DataType::UNSIGNED_BYTE:
                case DataType::UNSIGNED_BYTE_VECTOR2:
                case DataType::UNSIGNED_BYTE_VECTOR3:
                case DataType::UNSIGNED_BYTE_VECTOR4:
                    return GL_UNSIGNED_BYTE;

                case DataType::SHORT:
                case DataType::SHORT_VECTOR2:
                case DataType::SHORT_VECTOR3:
                case DataType::SHORT_VECTOR4:
                    return GL_SHORT;

                case DataType::UNSIGNED_SHORT:
                case DataType::UNSIGNED_SHORT_VECTOR2:
                case DataType::UNSIGNED_SHORT_VECTOR3:
                case DataType::UNSIGNED_SHORT_VECTOR4:
                    return GL_UNSIGNED_SHORT;

                case DataType::INTEGER:
                case DataType::INTEGER_VECTOR2:
                case DataType::INTEGER_VECTOR3:
                case DataType::INTEGER_VECTOR4:
                    return GL_INT;

                case DataType::UNSIGNED_INTEGER:
                case DataType::UNSIGNED_INTEGER_VECTOR2:
                case DataType::UNSIGNED_INTEGER_VECTOR3:
                case DataType::UNSIGNED_INTEGER_VECTOR4:
                    return GL_UNSIGNED_INT;

                case DataType::FLOAT:
                case DataType::FLOAT_VECTOR2:
                case DataType::FLOAT_VECTOR3:
                case DataType::FLOAT_VECTOR4:
                    return GL_FLOAT;

                default:
                    return 0;
            }
        }

        static GLint getArraySize(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::BYTE:
                case DataType::UNSIGNED_BYTE:
                case DataType::SHORT:
                case DataType::UNSIGNED_SHORT:
                case DataType::INTEGER:
                case DataType::UNSIGNED_INTEGER:
                case DataType::FLOAT:
                    return 1;

                case DataType::BYTE_VECTOR2:
                case DataType::UNSIGNED_BYTE_VECTOR2:
                case DataType::SHORT_VECTOR2:
                case DataType::UNSIGNED_SHORT_VECTOR2:
                case DataType::INTEGER_VECTOR2:
                case DataType::UNSIGNED_INTEGER_VECTOR2:
                case DataType::FLOAT_VECTOR2:
                    return 2;

                case DataType::BYTE_VECTOR3:
                case DataType::UNSIGNED_BYTE_VECTOR3:
                case DataType::SHORT_VECTOR3:
                case DataType::UNSIGNED_SHORT_VECTOR3:
                case DataType::INTEGER_VECTOR3:
                case DataType::UNSIGNED_INTEGER_VECTOR3:
                case DataType::FLOAT_VECTOR3:
                    return 3;

                case DataType::BYTE_VECTOR4:
                case DataType::UNSIGNED_BYTE_VECTOR4:
                case DataType::SHORT_VECTOR4:
                case DataType::UNSIGNED_SHORT_VECTOR4:
                case DataType::INTEGER_VECTOR4:
                case DataType::UNSIGNED_INTEGER_VECTOR4:
                case DataType::FLOAT_VECTOR4:
                    return 4;
                    
                default:
                    return 0;
            }
        }

        MeshBufferResourceOGL::MeshBufferResourceOGL(RenderDeviceOGL* aRenderDeviceOGL):
            renderDeviceOGL(aRenderDeviceOGL)
        {
        }

        MeshBufferResourceOGL::~MeshBufferResourceOGL()
        {
            if (vertexArrayId)
            {
                renderDeviceOGL->deleteVertexArray(vertexArrayId);
            }
        }

        bool MeshBufferResourceOGL::init(uint32_t newIndexSize, BufferResource* newIndexBuffer,
                                         const std::vector<VertexAttribute>& newVertexAttributes, BufferResource* newVertexBuffer)
        {
            if (!MeshBufferResource::init(newIndexSize, newIndexBuffer, newVertexAttributes, newVertexBuffer))
            {
                return false;
            }

            switch (indexSize)
            {
                case 2:
                    indexType = GL_UNSIGNED_SHORT;
                    bytesPerIndex = 2;
                    break;
                case 4:
                    indexType = GL_UNSIGNED_INT;
                    bytesPerIndex = 4;
                    break;
                default:
                    indexType = 0;
                    bytesPerIndex = 0;
                    Log(Log::Level::ERR) << "Invalid index size";
                    return false;
            }

            vertexAttribs.clear();

            GLuint offset = 0;

            for (const VertexAttribute& vertexAttribute : vertexAttributes)
            {
                GLboolean normalized = vertexAttribute.normalized ? GL_TRUE : GL_FALSE;

                vertexAttribs.push_back({
                    getArraySize(vertexAttribute.dataType), getVertexFormat(vertexAttribute.dataType), normalized,
                    static_cast<GLsizei>(vertexSize),
                    static_cast<const GLchar*>(nullptr) + offset
                });
                offset += getDataTypeSize(vertexAttribute.dataType);
            }

            if (offset != vertexSize)
            {
                Log(Log::Level::ERR) << "Invalid vertex size";
                return false;
            }

            if (!vertexArrayId)
            {
                if (glGenVertexArraysProc) glGenVertexArraysProc(1, &vertexArrayId);
            }

            if (RenderDeviceOGL::checkOpenGLError())
            {
                Log(Log::Level::WARN) << "Failed to create vertex array";
            }

            indexBufferOGL = static_cast<BufferResourceOGL*>(indexBuffer);
            vertexBufferOGL = static_cast<BufferResourceOGL*>(vertexBuffer);

            if (vertexArrayId)
            {
                renderDeviceOGL->bindVertexArray(vertexArrayId);

                if (indexBufferOGL && indexBufferOGL->getBufferId())
                {
                    if (!renderDeviceOGL->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferOGL->getBufferId()))
                    {
                        return false;
                    }
                }

                if (vertexBufferOGL && vertexBufferOGL->getBufferId())
                {
                    if (!renderDeviceOGL->bindBuffer(GL_ARRAY_BUFFER, vertexBufferOGL->getBufferId()))
                    {
                        return false;
                    }

                    for (GLuint index = 0; index < VERTEX_ATTRIBUTE_COUNT; ++index)
                    {
                        if (index < vertexAttribs.size())
                        {
                            glEnableVertexAttribArrayProc(index);
                            glVertexAttribPointerProc(index,
                                                      vertexAttribs[index].size,
                                                      vertexAttribs[index].type,
                                                      vertexAttribs[index].normalized,
                                                      vertexAttribs[index].stride,
                                                      vertexAttribs[index].pointer);
                        }
                        else
                        {
                            glDisableVertexAttribArrayProc(index);
                        }
                    }

                    if (RenderDeviceOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to update vertex attributes";
                        return false;
                    }
                }
            }

            return true;
        }

        bool MeshBufferResourceOGL::reload()
        {
            return true;
        }

        bool MeshBufferResourceOGL::setIndexSize(uint32_t newIndexSize)
        {
            if (MeshBufferResource::setIndexSize(newIndexSize))
            {
                return false;
            }

            switch (indexSize)
            {
                case 2:
                    indexType = GL_UNSIGNED_SHORT;
                    bytesPerIndex = 2;
                    break;
                case 4:
                    indexType = GL_UNSIGNED_INT;
                    bytesPerIndex = 4;
                    break;
                default:
                    indexType = 0;
                    bytesPerIndex = 0;
                    Log(Log::Level::ERR) << "Invalid index size";
                    return false;
            }

            return true;
        }

        bool MeshBufferResourceOGL::setIndexBuffer(BufferResource* newIndexBuffer)
        {
            if (MeshBufferResource::setIndexBuffer(newIndexBuffer))
            {
                return false;
            }

            indexBufferOGL = static_cast<BufferResourceOGL*>(indexBuffer);

            if (vertexArrayId)
            {
                renderDeviceOGL->bindVertexArray(vertexArrayId);

                if (indexBufferOGL && indexBufferOGL->getBufferId())
                {
                    if (!renderDeviceOGL->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferOGL->getBufferId()))
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        bool MeshBufferResourceOGL::setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes)
        {
            if (MeshBufferResource::setVertexAttributes(newVertexAttributes))
            {
                return false;
            }

            vertexAttribs.clear();

            GLuint offset = 0;

            for (const VertexAttribute& vertexAttribute : vertexAttributes)
            {
                GLboolean normalized = vertexAttribute.normalized ? GL_TRUE : GL_FALSE;

                vertexAttribs.push_back({
                    getArraySize(vertexAttribute.dataType), getVertexFormat(vertexAttribute.dataType), normalized,
                    static_cast<GLsizei>(vertexSize),
                    static_cast<const GLchar*>(nullptr) + offset
                });
                offset += getDataTypeSize(vertexAttribute.dataType);
            }

            if (offset != vertexSize)
            {
                Log(Log::Level::ERR) << "Invalid vertex size";
                return false;
            }

            if (vertexArrayId)
            {
                renderDeviceOGL->bindVertexArray(vertexArrayId);

                if (indexBufferOGL && indexBufferOGL->getBufferId())
                {
                    if (!renderDeviceOGL->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferOGL->getBufferId()))
                    {
                        return false;
                    }
                }

                if (vertexBufferOGL && vertexBufferOGL->getBufferId())
                {
                    if (!renderDeviceOGL->bindBuffer(GL_ARRAY_BUFFER, vertexBufferOGL->getBufferId()))
                    {
                        return false;
                    }

                    for (GLuint index = 0; index < VERTEX_ATTRIBUTE_COUNT; ++index)
                    {
                        if (index < vertexAttribs.size())
                        {
                            glEnableVertexAttribArrayProc(index);
                            glVertexAttribPointerProc(index,
                                                      vertexAttribs[index].size,
                                                      vertexAttribs[index].type,
                                                      vertexAttribs[index].normalized,
                                                      vertexAttribs[index].stride,
                                                      vertexAttribs[index].pointer);
                        }
                        else
                        {
                            glDisableVertexAttribArrayProc(index);
                        }
                    }

                    if (RenderDeviceOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to update vertex attributes";
                        return false;
                    }
                }
            }

            return true;
        }

        bool MeshBufferResourceOGL::setVertexBuffer(BufferResource* newVertexBuffer)
        {
            if (MeshBufferResource::setVertexBuffer(newVertexBuffer))
            {
                return false;
            }

            vertexBufferOGL = static_cast<BufferResourceOGL*>(vertexBuffer);

            if (vertexArrayId)
            {
                if (vertexBufferOGL && vertexBufferOGL->getBufferId())
                {
                    if (!renderDeviceOGL->bindBuffer(GL_ARRAY_BUFFER, vertexBufferOGL->getBufferId()))
                    {
                        return false;
                    }

                    for (GLuint index = 0; index < VERTEX_ATTRIBUTE_COUNT; ++index)
                    {
                        if (index < vertexAttribs.size())
                        {
                            glEnableVertexAttribArrayProc(index);
                            glVertexAttribPointerProc(index,
                                                      vertexAttribs[index].size,
                                                      vertexAttribs[index].type,
                                                      vertexAttribs[index].normalized,
                                                      vertexAttribs[index].stride,
                                                      vertexAttribs[index].pointer);
                        }
                        else
                        {
                            glDisableVertexAttribArrayProc(index);
                        }
                    }

                    if (RenderDeviceOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to update vertex attributes";
                        return false;
                    }
                }
            }

            return true;
        }

        bool MeshBufferResourceOGL::bindBuffers()
        {
            if (vertexArrayId)
            {
                if (!renderDeviceOGL->bindVertexArray(vertexArrayId))
                {
                    return false;
                }
            }
            else
            {
                // TODO: return false for OpenGL 3.1 and up
                if (!indexBufferOGL || !indexBufferOGL->getBufferId())
                {
                    Log(Log::Level::ERR) << "Index buffer not initialized";
                    return false;
                }

                if (!renderDeviceOGL->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferOGL->getBufferId()))
                {
                    return false;
                }

                if (!vertexBufferOGL || !vertexBufferOGL->getBufferId())
                {
                    Log(Log::Level::ERR) << "Vertex buffer not initialized";
                    return false;
                }

                if (!renderDeviceOGL->bindBuffer(GL_ARRAY_BUFFER, vertexBufferOGL->getBufferId()))
                {
                    return false;
                }

                for (GLuint index = 0; index < VERTEX_ATTRIBUTE_COUNT; ++index)
                {
                    if (index < vertexAttribs.size())
                    {
                        glEnableVertexAttribArrayProc(index);
                        glVertexAttribPointerProc(index,
                                                  vertexAttribs[index].size,
                                                  vertexAttribs[index].type,
                                                  vertexAttribs[index].normalized,
                                                  vertexAttribs[index].stride,
                                                  vertexAttribs[index].pointer);
                    }
                    else
                    {
                        glDisableVertexAttribArrayProc(index);
                    }
                }

                if (RenderDeviceOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to update vertex attributes";
                    return false;
                }
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
