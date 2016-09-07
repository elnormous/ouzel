// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "VertexBufferOGL.h"
#include "RendererOGL.h"

namespace ouzel
{
    namespace graphics
    {
        VertexBufferOGL::VertexBufferOGL()
        {
        }

        VertexBufferOGL::~VertexBufferOGL()
        {
            if (bufferId)
            {
                RendererOGL::deleteResource(bufferId, RendererOGL::ResourceType::Buffer);
            }
        }

        void VertexBufferOGL::free()
        {
            VertexBuffer::free();

            if (bufferId)
            {
                RendererOGL::deleteResource(bufferId, RendererOGL::ResourceType::Buffer);
                bufferId = 0;
            }
        }

        bool VertexBufferOGL::bindBuffer()
        {
            if (!bufferId)
            {
                log(LOG_LEVEL_ERROR, "Vertex buffer not initialized");
                return false;
            }

            if (!RendererOGL::bindArrayBuffer(bufferId))
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
                log(LOG_LEVEL_ERROR, "Failed to update vertex attributes");
                return false;
            }

            return true;
        }

        bool VertexBufferOGL::upload()
        {
            if (dirty)
            {
                if (!bufferId)
                {
                    glGenBuffers(1, &bufferId);
                }

                if (dirty & VERTEX_ATTRIBUTES_DIRTY)
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
                        log(LOG_LEVEL_ERROR, "Invalid vertex size");
                        return false;
                    }

                    dirty &= ~VERTEX_ATTRIBUTES_DIRTY;
                }

                if (dirty & VERTEX_BUFFER_DIRTY)
                {
                    if (!uploadData.data.empty())
                    {
                        RendererOGL::bindVertexArray(0);
                        RendererOGL::bindArrayBuffer(bufferId);

                        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(uploadData.data.size()), uploadData.data.data(),
                                     uploadData.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                        
                        if (RendererOGL::checkOpenGLError())
                        {
                            log(LOG_LEVEL_ERROR, "Failed to create vertex data");
                            return false;
                        }
                    }
                    
                    dirty &= ~VERTEX_BUFFER_DIRTY;
                }
                
                dirty = 0;
                ready = true;
            }
            
            return true;
        }
    }
}
