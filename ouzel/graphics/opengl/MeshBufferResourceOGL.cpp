// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#include "MeshBufferResourceOGL.h"
#include "RendererOGL.h"
#include "BufferResourceOGL.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferResourceOGL::MeshBufferResourceOGL()
        {
        }

        MeshBufferResourceOGL::~MeshBufferResourceOGL()
        {
            if (vertexArrayId)
            {
                RendererOGL::deleteVertexArray(vertexArrayId);
            }
        }

        bool MeshBufferResourceOGL::bindBuffers()
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
                if (!indexBufferOGL || !indexBufferOGL->getBufferId())
                {
                    Log(Log::Level::ERR) << "Index buffer not initialized";
                    return false;
                }

                if (!RendererOGL::bindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferOGL->getBufferId()))
                {
                    return false;
                }

                if (!vertexBufferOGL || !vertexBufferOGL->getBufferId())
                {
                    Log(Log::Level::ERR) << "Vertex buffer not initialized";
                    return false;
                }

                if (!RendererOGL::bindBuffer(GL_ARRAY_BUFFER, vertexBufferOGL->getBufferId()))
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

        bool MeshBufferResourceOGL::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
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

                if (RendererOGL::checkOpenGLError())
                {
                    Log(Log::Level::WARN) << "Failed to create vertex array";
                }

                indexBufferOGL = static_cast<BufferResourceOGL*>(indexBuffer);

                if (indexBufferOGL && !indexBufferOGL->upload())
                {
                    return false;
                }

                vertexBufferOGL = static_cast<BufferResourceOGL*>(vertexBuffer);

                if (vertexBufferOGL && !vertexBufferOGL->upload())
                {
                    return false;
                }

                if (vertexArrayId)
                {
                    RendererOGL::bindVertexArray(vertexArrayId);

                    if (indexBufferOGL && indexBufferOGL->getBufferId())
                    {
                        if (!RendererOGL::bindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferOGL->getBufferId()))
                        {
                            return false;
                        }
                    }

                    if (vertexBufferOGL && vertexBufferOGL->getBufferId())
                    {
                        if (!RendererOGL::bindBuffer(GL_ARRAY_BUFFER, vertexBufferOGL->getBufferId()))
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

                        if (RendererOGL::checkOpenGLError())
                        {
                            Log(Log::Level::ERR) << "Failed to update vertex attributes";
                            return false;
                        }
                    }
                }

                dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
