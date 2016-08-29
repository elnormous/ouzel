// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferOGL.h"
#include "core/CompileConfig.h"
#include "core/Engine.h"
#include "RendererOGL.h"
#include "utils/Utils.h"

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
                RendererOGL::deleteResource(vertexArrayId, RendererOGL::ResourceType::VertexArray);
            }

            if (vertexBufferId)
            {
                RendererOGL::deleteResource(vertexBufferId, RendererOGL::ResourceType::Buffer);
            }

            if (indexBufferId)
            {
                RendererOGL::deleteResource(indexBufferId, RendererOGL::ResourceType::Buffer);
            }
        }

        void MeshBufferOGL::free()
        {
            MeshBuffer::free();

            if (vertexArrayId)
            {
                RendererOGL::deleteResource(vertexArrayId, RendererOGL::ResourceType::VertexArray);
                vertexArrayId = 0;
            }

            if (vertexBufferId)
            {
                RendererOGL::deleteResource(vertexBufferId, RendererOGL::ResourceType::Buffer);
                vertexBufferId = 0;
            }

            if (indexBufferId)
            {
                RendererOGL::deleteResource(indexBufferId, RendererOGL::ResourceType::Buffer);
                indexBufferId = 0;
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
                if (!vertexBufferId)
                {
                    log("Vertex buffer not initialized");
                    return false;
                }

                if (!RendererOGL::bindArrayBuffer(vertexBufferId))
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
                    log("Failed to update vertex attributes");
                    return false;
                }

                if (!indexBufferId)
                {
                    log("Index buffer not initialized");
                    return false;
                }

                if (!RendererOGL::bindElementArrayBuffer(indexBufferId))
                {
                    return false;
                }
            }

            return true;
        }

        bool MeshBufferOGL::upload()
        {
            if (indexBufferDirty || vertexBufferDirty || indexSizeDirty || vertexAttributesDirty)
            {
                if (indexSizeDirty)
                {
                    switch (uploadData.indexSize)
                    {
                        case 2:
                            indexFormat = GL_UNSIGNED_SHORT;
                            bytesPerIndex = 2;
                            break;
                        case 4:
                            indexFormat = GL_UNSIGNED_INT;
                            bytesPerIndex = 4;
                            break;
                        default:
                            indexFormat = 0;
                            bytesPerIndex = 0;
                            log("Invalid index size");
                            return false;
                    }

                    indexSizeDirty = false;
                }

                if (!vertexArrayId)
                {
#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
                    glGenVertexArraysOES(1, &vertexArrayId);
#elif OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN
                    if (glGenVertexArraysOESEXT) glGenVertexArraysOESEXT(1, &vertexArrayId);
#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX
                    glGenVertexArrays(1, &vertexArrayId);
#endif
                }

                if (!indexBufferId)
                {
                    glGenBuffers(1, &indexBufferId);
                }

                if (indexBufferDirty)
                {
                    if (!uploadData.indexData.empty())
                    {
                        if (vertexArrayId)
                        {
                            RendererOGL::bindVertexArray(vertexArrayId);
                            RendererOGL::bindElementArrayBuffer(indexBufferId);
                        }
                        else
                        {
                            RendererOGL::bindElementArrayBuffer(indexBufferId);
                        }

                        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(uploadData.indexData.size()), uploadData.indexData.data(),
                                     uploadData.dynamicIndexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                        if (RendererOGL::checkOpenGLError())
                        {
                            log("Failed to upload index data");
                            return false;
                        }
                    }

                    indexBufferDirty = false;
                }

                if (!vertexBufferId)
                {
                    glGenBuffers(1, &vertexBufferId);
                }

                if (vertexAttributesDirty)
                {
                    vertexAttribs.clear();

                    GLuint offset = 0;

                    if (uploadData.vertexAttributes & VERTEX_POSITION)
                    {
                        vertexAttribs.push_back({
                            3, GL_FLOAT, GL_FALSE,
                            static_cast<GLsizei>(uploadData.vertexSize),
                            reinterpret_cast<const GLvoid*>(offset)
                        });
                        offset += 3 * sizeof(float);
                    }
                    if (uploadData.vertexAttributes & VERTEX_COLOR)
                    {
                        vertexAttribs.push_back({
                            4, GL_UNSIGNED_BYTE, GL_TRUE,
                            static_cast<GLsizei>(uploadData.vertexSize),
                            reinterpret_cast<const GLvoid*>(offset)
                        });
                        offset += 4 * sizeof(uint8_t);
                    }
                    if (uploadData.vertexAttributes & VERTEX_NORMAL)
                    {
                        vertexAttribs.push_back({
                            3, GL_FLOAT, GL_FALSE,
                            static_cast<GLsizei>(uploadData.vertexSize),
                            reinterpret_cast<const GLvoid*>(offset)
                        });
                        offset += 3 * sizeof(float);
                    }
                    if (uploadData.vertexAttributes & VERTEX_TEXCOORD0)
                    {
                        vertexAttribs.push_back({
                            2, GL_FLOAT, GL_FALSE,
                            static_cast<GLsizei>(uploadData.vertexSize),
                            reinterpret_cast<const GLvoid*>(offset)
                        });
                        offset += 2 * sizeof(float);
                    }
                    if (uploadData.vertexAttributes & VERTEX_TEXCOORD1)
                    {
                        vertexAttribs.push_back({
                            2, GL_FLOAT, GL_FALSE,
                            static_cast<GLsizei>(uploadData.vertexSize),
                            reinterpret_cast<const GLvoid*>(offset)
                        });
                        offset += 2 * sizeof(float);
                    }
                    
                    if (offset != uploadData.vertexSize)
                    {
                        log("Invalid vertex size");
                        return false;
                    }

                    if (vertexArrayId)
                    {
                        RendererOGL::bindVertexArray(vertexArrayId);
                        RendererOGL::bindArrayBuffer(vertexBufferId);

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
                            log("Failed to update vertex attributes");
                            return false;
                        }
                    }

                    vertexAttributesDirty = false;
                }

                if (vertexBufferDirty)
                {
                    if (!uploadData.vertexData.empty())
                    {
                        if (vertexArrayId)
                        {
                            RendererOGL::bindVertexArray(vertexArrayId);
                            RendererOGL::bindArrayBuffer(vertexBufferId);
                        }
                        else
                        {
                            RendererOGL::bindArrayBuffer(vertexBufferId);
                        }

                        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(uploadData.vertexData.size()), uploadData.vertexData.data(),
                                     uploadData.dynamicVertexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                        if (RendererOGL::checkOpenGLError())
                        {
                            log("Failed to create vertex data");
                            return false;
                        }
                    }

                    vertexBufferDirty = false;
                }

                ready = true;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
