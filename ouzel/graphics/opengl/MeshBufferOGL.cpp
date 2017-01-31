// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferOGL.h"
#include "core/CompileConfig.h"
#include "core/Engine.h"
#include "RendererOGL.h"
#include "BufferOGL.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferOGL::MeshBufferOGL()
        {
        }

        MeshBufferOGL::~MeshBufferOGL()
        {
            if (vertexArrayId)
            {
                RendererOGL::deleteVertexArray(vertexArrayId);
            }
        }

        bool MeshBufferOGL::bindBuffers()
        {
            if (vertexArrayId)
            {
                if (!RendererOGL::bindVertexArray(vertexArrayId))
                {
                    return false;
                }
            }
            else
            {
                // TODO: return false for OpenGL 3.1 and up 

                BufferOGL* indexBufferOGL = static_cast<BufferOGL*>(data.indexBuffer);

                if (!indexBufferOGL || !indexBufferOGL->bindBuffer())
                {
                    return false;
                }

                BufferOGL* vertexBufferOGL = static_cast<BufferOGL*>(data.vertexBuffer);

                if (!vertexBufferOGL || !vertexBufferOGL->bindBuffer())
                {
                    return false;
                }

                for (GLuint index = 0; index < VERTEX_ATTRIBUTE_COUNT; ++index)
                {
                    if (index < vertexAttribs.size())
                    {
                        glEnableVertexAttribArray(index);
                        glVertexAttribPointer(index,
                                              vertexAttribs[index].size,
                                              vertexAttribs[index].type,
                                              vertexAttribs[index].normalized,
                                              vertexAttribs[index].stride,
                                              vertexAttribs[index].pointer);
                    }
                    else
                    {
                        glDisableVertexAttribArray(index);
                    }
                }

                if (RendererOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to update vertex attributes";
                    return false;
                }
            }

            return true;
        }

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

        bool MeshBufferOGL::upload()
        {
            if (!MeshBufferResource::upload())
            {
                return false;
            }

            if (data.dirty)
            {
                switch (data.indexSize)
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

                for (const VertexAttribute& vertexAttribute : data.vertexAttributes)
                {
                    GLboolean normalized = vertexAttribute.normalized ? GL_TRUE : GL_FALSE;

                    vertexAttribs.push_back({
                        3, getVertexFormat(vertexAttribute.dataType), normalized,
                        static_cast<GLsizei>(data.vertexSize),
                        reinterpret_cast<const GLvoid*>(offset)
                    });
                    offset += getDataTypeSize(vertexAttribute.dataType);
                }

                if (offset != data.vertexSize)
                {
                    Log(Log::Level::ERR) << "Invalid vertex size";
                    return false;
                }

                if (!vertexArrayId)
                {
    #if OUZEL_OPENGL_INTERFACE_EAGL
                    glGenVertexArraysOES(1, &vertexArrayId);
    #elif OUZEL_OPENGL_INTERFACE_EGL
                    if (glGenVertexArraysProc) glGenVertexArraysProc(1, &vertexArrayId);
    #elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX
                    glGenVertexArrays(1, &vertexArrayId);
    #endif
                }

                if (RendererOGL::checkOpenGLError())
                {
                    Log(Log::Level::WARN) << "Failed to create vertex array";
                }

                BufferOGL* indexBufferOGL = static_cast<BufferOGL*>(data.indexBuffer);

                if (indexBufferOGL && !indexBufferOGL->upload())
                {
                    return false;
                }

                BufferOGL* vertexBufferOGL = static_cast<BufferOGL*>(data.vertexBuffer);

                if (vertexBufferOGL && !vertexBufferOGL->upload())
                {
                    return false;
                }

                if (vertexArrayId)
                {
                    RendererOGL::bindVertexArray(vertexArrayId);

                    if (indexBufferOGL && !indexBufferOGL->bindBuffer())
                    {
                        return false;
                    }

                    if (vertexBufferOGL)
                    {
                        if (!vertexBufferOGL->bindBuffer())
                        {
                            return false;
                        }

                        for (GLuint index = 0; index < VERTEX_ATTRIBUTE_COUNT; ++index)
                        {
                            if (index < vertexAttribs.size())
                            {
                                glEnableVertexAttribArray(index);
                                glVertexAttribPointer(index,
                                                      vertexAttribs[index].size,
                                                      vertexAttribs[index].type,
                                                      vertexAttribs[index].normalized,
                                                      vertexAttribs[index].stride,
                                                      vertexAttribs[index].pointer);
                            }
                            else
                            {
                                glDisableVertexAttribArray(index);
                            }
                        }

                        if (RendererOGL::checkOpenGLError())
                        {
                            Log(Log::Level::ERR) << "Failed to update vertex attributes";
                            return false;
                        }
                    }
                }

                data.dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
