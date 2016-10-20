// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "IndexBufferOGL.h"
#include "RendererOGL.h"
#include "utils/Log.h"

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
                Log(Log::Level::ERR) << "Index buffer not initialized";
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
            if (uploadData.dirty)
            {
                if (!bufferId)
                {
                    glGenBuffers(1, &bufferId);
                }

                if (uploadData.dirty & INDEX_SIZE_DIRTY)
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
                            Log(Log::Level::ERR) << "Invalid index size";
                            return false;
                    }

                    uploadData.dirty &= ~INDEX_SIZE_DIRTY;
                }

                if (uploadData.dirty & INDEX_BUFFER_DIRTY)
                {
                    if (!uploadData.data.empty())
                    {
                        RendererOGL::bindVertexArray(0);
                        RendererOGL::bindElementArrayBuffer(bufferId);

                        if (static_cast<GLsizeiptr>(uploadData.data.size()) > bufferSize)
                        {
                            bufferSize = static_cast<GLsizeiptr>(uploadData.data.size());

                            glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, nullptr,
                                         uploadData.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                            if (RendererOGL::checkOpenGLError())
                            {
                                Log(Log::Level::ERR) << "Failed to create index buffer";
                                return false;
                            }
                        }

                        void* bufferPtr;

#if OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
    #if defined(GL_EXT_map_buffer_range)
                        bufferPtr = mapBufferRangeEXT ? mapBufferRangeEXT(GL_ELEMENT_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(uploadData.data.size()), GL_MAP_UNSYNCHRONIZED_BIT_EXT | GL_MAP_WRITE_BIT_EXT) : nullptr;
    #elif defined(GL_OES_mapbuffer)
                        bufferPtr = mapBufferOES ? mapBufferOES(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY_OES) : nullptr;
    #else
                        bufferPtr = nullptr;
    #endif
#else
                        bufferPtr = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(uploadData.data.size()), GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT);
#endif

                        if (bufferPtr)
                        {
                            std::copy(uploadData.data.begin(), uploadData.data.end(), static_cast<uint8_t*>(bufferPtr));
                        }
                        else
                        {
                            // glMapBufferRange failed
                            if (RendererOGL::checkOpenGLError())
                            {
                                Log(Log::Level::ERR) << "Failed to map index buffer";
                                return false;
                            }

                            glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, uploadData.data.data(),
                                         uploadData.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                            if (RendererOGL::checkOpenGLError())
                            {
                                Log(Log::Level::ERR) << "Failed to upload index buffer";
                                return false;
                            }
                        }

#if OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
    #if defined(GL_OES_mapbuffer)
                        if (unmapBufferOES) unmapBufferOES(GL_ELEMENT_ARRAY_BUFFER);
    #endif
#else
                        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
#endif

                        if (RendererOGL::checkOpenGLError())
                        {
                            Log(Log::Level::ERR) << "Failed to upload index buffer";
                            return false;
                        }
                    }

                    uploadData.dirty &= ~INDEX_BUFFER_DIRTY;
                }

                uploadData.dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
