// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "MeshBufferResourceOGL.hpp"
#include "RenderDeviceOGL.hpp"
#include "BufferResourceOGL.hpp"
#include "utils/Log.hpp"

static const uint32_t MAX_VERTEX_ATTRIBUTE_COUNT = 5;

namespace ouzel
{
    namespace graphics
    {
        static GLenum getVertexFormat(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::BYTE:
                case DataType::BYTE_NORM:
                case DataType::BYTE_VECTOR2:
                case DataType::BYTE_VECTOR2_NORM:
                case DataType::BYTE_VECTOR3:
                case DataType::BYTE_VECTOR3_NORM:
                case DataType::BYTE_VECTOR4:
                case DataType::BYTE_VECTOR4_NORM:
                    return GL_BYTE;

                case DataType::UNSIGNED_BYTE:
                case DataType::UNSIGNED_BYTE_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR2:
                case DataType::UNSIGNED_BYTE_VECTOR2_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR3:
                case DataType::UNSIGNED_BYTE_VECTOR3_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR4:
                case DataType::UNSIGNED_BYTE_VECTOR4_NORM:
                    return GL_UNSIGNED_BYTE;

                case DataType::SHORT:
                case DataType::SHORT_NORM:
                case DataType::SHORT_VECTOR2:
                case DataType::SHORT_VECTOR2_NORM:
                case DataType::SHORT_VECTOR3:
                case DataType::SHORT_VECTOR3_NORM:
                case DataType::SHORT_VECTOR4:
                case DataType::SHORT_VECTOR4_NORM:
                    return GL_SHORT;

                case DataType::UNSIGNED_SHORT:
                case DataType::UNSIGNED_SHORT_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR2:
                case DataType::UNSIGNED_SHORT_VECTOR2_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR3:
                case DataType::UNSIGNED_SHORT_VECTOR3_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR4:
                case DataType::UNSIGNED_SHORT_VECTOR4_NORM:
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
                case DataType::FLOAT_MATRIX3:
                case DataType::FLOAT_MATRIX4:
                    return GL_FLOAT;

                default:
                    return 0;
            }

            return 0;
        }

        static GLint getArraySize(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::BYTE:
                case DataType::BYTE_NORM:
                case DataType::UNSIGNED_BYTE:
                case DataType::UNSIGNED_BYTE_NORM:
                case DataType::SHORT:
                case DataType::SHORT_NORM:
                case DataType::UNSIGNED_SHORT:
                case DataType::UNSIGNED_SHORT_NORM:
                case DataType::INTEGER:
                case DataType::UNSIGNED_INTEGER:
                case DataType::FLOAT:
                    return 1;

                case DataType::BYTE_VECTOR2:
                case DataType::BYTE_VECTOR2_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR2:
                case DataType::UNSIGNED_BYTE_VECTOR2_NORM:
                case DataType::SHORT_VECTOR2:
                case DataType::SHORT_VECTOR2_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR2:
                case DataType::UNSIGNED_SHORT_VECTOR2_NORM:
                case DataType::INTEGER_VECTOR2:
                case DataType::UNSIGNED_INTEGER_VECTOR2:
                case DataType::FLOAT_VECTOR2:
                    return 2;

                case DataType::BYTE_VECTOR3:
                case DataType::BYTE_VECTOR3_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR3:
                case DataType::UNSIGNED_BYTE_VECTOR3_NORM:
                case DataType::SHORT_VECTOR3:
                case DataType::SHORT_VECTOR3_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR3:
                case DataType::UNSIGNED_SHORT_VECTOR3_NORM:
                case DataType::INTEGER_VECTOR3:
                case DataType::UNSIGNED_INTEGER_VECTOR3:
                case DataType::FLOAT_VECTOR3:
                    return 3;

                case DataType::BYTE_VECTOR4:
                case DataType::BYTE_VECTOR4_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR4:
                case DataType::UNSIGNED_BYTE_VECTOR4_NORM:
                case DataType::SHORT_VECTOR4:
                case DataType::SHORT_VECTOR4_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR4:
                case DataType::UNSIGNED_SHORT_VECTOR4_NORM:
                case DataType::INTEGER_VECTOR4:
                case DataType::UNSIGNED_INTEGER_VECTOR4:
                case DataType::FLOAT_VECTOR4:
                    return 4;

                case DataType::FLOAT_MATRIX3:
                    return 3 * 3;

                case DataType::FLOAT_MATRIX4:
                    return 4 * 4;
;
                default:
                    return 0;
            }

            return 0;
        }

        static GLboolean isNormalized(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::BYTE_NORM:
                case DataType::BYTE_VECTOR2_NORM:
                case DataType::BYTE_VECTOR3_NORM:
                case DataType::BYTE_VECTOR4_NORM:
                case DataType::UNSIGNED_BYTE_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR2_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR3_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR4_NORM:
                case DataType::SHORT_NORM:
                case DataType::SHORT_VECTOR2_NORM:
                case DataType::SHORT_VECTOR3_NORM:
                case DataType::SHORT_VECTOR4_NORM:
                case DataType::UNSIGNED_SHORT_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR2_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR3_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR4_NORM:
                    return GL_TRUE;
                default:
                    return GL_FALSE;
            }
        }

        MeshBufferResourceOGL::MeshBufferResourceOGL(RenderDeviceOGL* initRenderDeviceOGL):
            renderDeviceOGL(initRenderDeviceOGL)
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
                                         BufferResource* newVertexBuffer)
        {
            if (!MeshBufferResource::init(newIndexSize, newIndexBuffer, newVertexBuffer))
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

            for (const Vertex::Attribute& vertexAttribute : Vertex::ATTRIBUTES)
            {
                vertexAttribs.push_back({
                    getArraySize(vertexAttribute.dataType),
                    getVertexFormat(vertexAttribute.dataType),
                    isNormalized(vertexAttribute.dataType),
                    static_cast<GLsizei>(sizeof(Vertex)),
                    static_cast<const GLchar*>(nullptr) + offset
                });
                offset += getDataTypeSize(vertexAttribute.dataType);
            }

            indexBufferOGL = static_cast<BufferResourceOGL*>(indexBuffer);
            vertexBufferOGL = static_cast<BufferResourceOGL*>(vertexBuffer);

            if (vertexArrayId)
            {
                if (glDeleteVertexArraysProc) glDeleteVertexArraysProc(1, &vertexArrayId);
            }

            return createVertexArray();
        }

        bool MeshBufferResourceOGL::reload()
        {
            vertexArrayId = 0;

            return createVertexArray();
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

                    for (GLuint index = 0; index < MAX_VERTEX_ATTRIBUTE_COUNT; ++index)
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

                for (GLuint index = 0; index < MAX_VERTEX_ATTRIBUTE_COUNT; ++index)
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

        bool MeshBufferResourceOGL::createVertexArray()
        {
            if (glGenVertexArraysProc) glGenVertexArraysProc(1, &vertexArrayId);

            if (RenderDeviceOGL::checkOpenGLError())
            {
                Log(Log::Level::WARN) << "Failed to create vertex array";
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

                    for (GLuint index = 0; index < MAX_VERTEX_ATTRIBUTE_COUNT; ++index)
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
    } // namespace graphics
} // namespace ouzel

#endif
