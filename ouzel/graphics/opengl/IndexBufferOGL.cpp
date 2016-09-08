// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstring>
#include "IndexBufferOGL.h"
#include "RendererOGL.h"

namespace ouzel
{
    namespace graphics
    {
        IndexBufferOGL::IndexBufferOGL()
        {
        }

        IndexBufferOGL::~IndexBufferOGL()
        {
            if (bufferId)
            {
                RendererOGL::deleteResource(bufferId, RendererOGL::ResourceType::Buffer);
            }
        }

        void IndexBufferOGL::free()
        {
            IndexBuffer::free();

            if (bufferId)
            {
                RendererOGL::deleteResource(bufferId, RendererOGL::ResourceType::Buffer);
                bufferId = 0;
            }

            bufferSize = 0;
        }

        bool IndexBufferOGL::bindBuffer()
        {
            if (!bufferId)
            {
                log(LOG_LEVEL_ERROR, "Index buffer not initialized");
                return false;
            }

            if (!RendererOGL::bindElementArrayBuffer(bufferId))
            {
                return false;
            }

            return true;
        }

        bool IndexBufferOGL::upload()
        {
            if (dirty)
            {
                if (!bufferId)
                {
                    glGenBuffers(1, &bufferId);
                }

                if (dirty & INDEX_SIZE_DIRTY)
                {
                    switch (uploadData.indexSize)
                    {
                        case 2:
                            type = GL_UNSIGNED_SHORT;
                            bytesPerIndex = 2;
                            break;
                        case 4:
                            type = GL_UNSIGNED_INT;
                            bytesPerIndex = 4;
                            break;
                        default:
                            type = 0;
                            bytesPerIndex = 0;
                            log(LOG_LEVEL_ERROR, "Invalid index size");
                            return false;
                    }

                    dirty &= ~INDEX_SIZE_DIRTY;
                }

                if (dirty & INDEX_BUFFER_DIRTY)
                {
                    if (!uploadData.data.empty())
                    {
                        RendererOGL::bindVertexArray(0);
                        RendererOGL::bindElementArrayBuffer(bufferId);

                        if (static_cast<GLsizeiptr>(uploadData.data.size()) > bufferSize)
                        {
                            bufferSize = static_cast<GLsizeiptr>(uploadData.data.size());

                            glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(uploadData.data.size()), uploadData.data.data(),
                                         uploadData.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                        }
                        else
                        {
#ifdef OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN
    #if GL_EXT_map_buffer_range
                            void* bufferPtr = glMapBufferRangeEXT(GL_ELEMENT_ARRAY_BUFFER, 0, uploadData.data.size(), GL_MAP_UNSYNCHRONIZED_BIT_EXT | GL_MAP_WRITE_BIT_EXT);
    #elif GL_OES_mapbuffer
                            void* bufferPtr = glMapBufferOES(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
    #else
                            log(LOG_LEVEL_ERROR, "OpenGL buffer mapping not supported");
                            return false;
    #endif
#else
                            void* bufferPtr = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, uploadData.data.size(), GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT);
#endif

                            memcpy(bufferPtr, uploadData.data.data(), uploadData.data.size());

#ifdef OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN
    #if GL_EXT_map_buffer_range || GL_OES_mapbuffer
                            glUnmapBufferOES(GL_ELEMENT_ARRAY_BUFFER);
    #endif
#else
                            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
#endif
                        }

                        if (RendererOGL::checkOpenGLError())
                        {
                            log(LOG_LEVEL_ERROR, "Failed to upload index data");
                            return false;
                        }
                    }

                    dirty &= ~INDEX_BUFFER_DIRTY;
                }
                
                dirty = 0;
                ready = true;
            }
            
            return true;
        }
    } // namespace graphics
} // namespace ouzel
