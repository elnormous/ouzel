// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

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

                        if (uploadData.data.size() > bufferSize)
                        {
                            bufferSize = static_cast<GLsizeiptr>(uploadData.data.size());

                            glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(uploadData.data.size()), uploadData.data.data(),
                                         uploadData.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                        }
                        else
                        {
                            void* bufferPtr = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, uploadData.data.size(), GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT);

                            memcpy(bufferPtr, uploadData.data.data(), uploadData.data.size());

                            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
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
