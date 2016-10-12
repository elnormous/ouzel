// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstring>
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

            bufferSize = 0;
        }

        bool VertexBufferOGL::bindBuffer()
        {
            if (!bufferId)
            {
                Log(Log::Level::ERR) << "Vertex buffer not initialized";
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
                Log(Log::Level::ERR) << "Failed to update vertex attributes";
                return false;
            }

            return true;
        }

        bool VertexBufferOGL::upload()
        {
            if (uploadData.dirty)
            {
                if (!bufferId)
                {
                    glGenBuffers(1, &bufferId);
                }

                if (uploadData.dirty & VERTEX_ATTRIBUTES_DIRTY)
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
                        Log(Log::Level::ERR) << "Invalid vertex size";
                        return false;
                    }

                    uploadData.dirty &= ~VERTEX_ATTRIBUTES_DIRTY;
                }

                if (uploadData.dirty & VERTEX_BUFFER_DIRTY)
                {
                    if (!uploadData.data.empty())
                    {
                        RendererOGL::bindVertexArray(0);
                        RendererOGL::bindArrayBuffer(bufferId);

                        if (static_cast<GLsizeiptr>(uploadData.data.size()) > bufferSize)
                        {
                            bufferSize = static_cast<GLsizeiptr>(uploadData.data.size());

                            glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr,
                                         uploadData.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                            if (RendererOGL::checkOpenGLError())
                            {
                                Log(Log::Level::ERR) << "Failed to create vertex buffer";
                                return false;
                            }
                        }

                        void* bufferPtr;

#if OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
    #if defined(GL_EXT_map_buffer_range)
                        bufferPtr = mapBufferRangeEXT ? mapBufferRangeEXT(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(uploadData.data.size()), GL_MAP_UNSYNCHRONIZED_BIT_EXT | GL_MAP_WRITE_BIT_EXT) : nullptr;
    #elif defined(GL_OES_mapbuffer)
                        bufferPtr = mapBufferOES ? mapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES) : nullptr;
    #else
                        bufferPtr = nullptr;
    #endif
#else
                        bufferPtr = glMapBufferRange(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(uploadData.data.size()), GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT);
#endif

                        if (bufferPtr)
                        {
                            memcpy(bufferPtr, uploadData.data.data(), uploadData.data.size());
                        }
                        else
                        {
                            // glMapBufferRange failed
                            if (RendererOGL::checkOpenGLError())
                            {
                                Log(Log::Level::ERR) << "Failed to map vertex buffer";
                                return false;
                            }

                            glBufferData(GL_ARRAY_BUFFER, bufferSize, uploadData.data.data(),
                                         uploadData.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                            if (RendererOGL::checkOpenGLError())
                            {
                                Log(Log::Level::ERR) << "Failed to upload vertex buffer";
                                return false;
                            }
                        }

#if OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
    #if defined(GL_OES_mapbuffer)
                        if (unmapBufferOES) unmapBufferOES(GL_ARRAY_BUFFER);
    #endif
#else
                        glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
                    }

                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to upload vertex buffer";
                        return false;
                    }

                    uploadData.dirty &= ~VERTEX_BUFFER_DIRTY;
                }

                uploadData.dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
