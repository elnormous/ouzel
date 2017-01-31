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

                if (data.vertexAttributes & VERTEX_POSITION)
                {
                    vertexAttribs.push_back({
                        3, GL_FLOAT, GL_FALSE,
                        static_cast<GLsizei>(data.vertexSize),
                        reinterpret_cast<const GLvoid*>(offset)
                    });
                    offset += 3 * sizeof(float);
                }
                if (data.vertexAttributes & VERTEX_COLOR)
                {
                    vertexAttribs.push_back({
                        4, GL_UNSIGNED_BYTE, GL_TRUE,
                        static_cast<GLsizei>(data.vertexSize),
                        reinterpret_cast<const GLvoid*>(offset)
                    });
                    offset += 4 * sizeof(uint8_t);
                }
                if (data.vertexAttributes & VERTEX_NORMAL)
                {
                    vertexAttribs.push_back({
                        3, GL_FLOAT, GL_FALSE,
                        static_cast<GLsizei>(data.vertexSize),
                        reinterpret_cast<const GLvoid*>(offset)
                    });
                    offset += 3 * sizeof(float);
                }
                if (data.vertexAttributes & VERTEX_TEXCOORD0)
                {
                    vertexAttribs.push_back({
                        2, GL_FLOAT, GL_FALSE,
                        static_cast<GLsizei>(data.vertexSize),
                        reinterpret_cast<const GLvoid*>(offset)
                    });
                    offset += 2 * sizeof(float);
                }
                if (data.vertexAttributes & VERTEX_TEXCOORD1)
                {
                    vertexAttribs.push_back({
                        2, GL_FLOAT, GL_FALSE,
                        static_cast<GLsizei>(data.vertexSize),
                        reinterpret_cast<const GLvoid*>(offset)
                    });
                    offset += 2 * sizeof(float);
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
