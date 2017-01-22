// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "BufferOGL.h"
#include "RendererOGL.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        BufferOGL::BufferOGL()
        {
        }

        BufferOGL::~BufferOGL()
        {
            if (bufferId)
            {
                RendererOGL::deleteResource(bufferId, RendererOGL::ResourceType::Buffer);
            }
        }

        void BufferOGL::free()
        {
            BufferResource::free();

            if (bufferId)
            {
                RendererOGL::deleteResource(bufferId, RendererOGL::ResourceType::Buffer);
                bufferId = 0;
            }

            bufferSize = 0;
        }

        bool BufferOGL::bindBuffer()
        {
            if (!bufferId)
            {
                Log(Log::Level::ERR) << "Buffer not initialized";
                return false;
            }

            if (!RendererOGL::bindBuffer(bufferType, bufferId))
            {
                return false;
            }

            return true;
        }

        bool BufferOGL::upload()
        {
            if (!BufferResource::upload())
            {
                return false;
            }

            if (!bufferId)
            {
                glGenBuffers(1, &bufferId);
            }

            switch (uploadData.usage)
            {
                case Buffer::Usage::INDEX:
                    bufferType = GL_ELEMENT_ARRAY_BUFFER;
                    break;
                case Buffer::Usage::VERTEX:
                    bufferType = GL_ARRAY_BUFFER;
                    break;
                default:
                    bufferType = 0;
                    Log(Log::Level::ERR) << "Unsupported buffer type";
                    return false;
            }

            if (!uploadData.data.empty())
            {
                RendererOGL::bindVertexArray(0);

                if (!RendererOGL::bindBuffer(bufferType, bufferId))
                {
                    return false;
                }

                if (static_cast<GLsizeiptr>(uploadData.data.size()) > bufferSize)
                {
                    bufferSize = static_cast<GLsizeiptr>(uploadData.data.size());

                    glBufferData(bufferType, bufferSize, nullptr,
                                 uploadData.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to create buffer";
                        return false;
                    }
                }

                void* bufferPtr = nullptr;

#if OUZEL_OPENGL_INTERFACE_EGL
#if defined(GL_EXT_map_buffer_range)
                if (mapBufferRangeEXT)
                {
                    bufferPtr = mapBufferRangeEXT(bufferType, 0, static_cast<GLsizeiptr>(uploadData.data.size()), GL_MAP_UNSYNCHRONIZED_BIT_EXT | GL_MAP_WRITE_BIT_EXT);
                }
#endif
#if defined(GL_OES_mapbuffer)
                if (!bufferPtr && mapBufferOES)
                {
                    bufferPtr = mapBufferOES(bufferType, GL_WRITE_ONLY_OES);
                }
#endif
#else
                bufferPtr = glMapBufferRange(bufferType, 0, static_cast<GLsizeiptr>(uploadData.data.size()), GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT);
#endif

                if (bufferPtr)
                {
                    std::copy(uploadData.data.begin(), uploadData.data.end(), static_cast<uint8_t*>(bufferPtr));

#if OUZEL_OPENGL_INTERFACE_EGL
#if defined(GL_OES_mapbuffer)
                    if (unmapBufferOES) unmapBufferOES(bufferType);
#endif
#else
                    glUnmapBuffer(bufferType);
#endif

                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to upload buffer";
                        return false;
                    }
                }
                else
                {
                    // glMapBufferRange failed
                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to map buffer";
                        return false;
                    }

                    glBufferData(bufferType, bufferSize, uploadData.data.data(),
                                 uploadData.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to upload buffer";
                        return false;
                    }
                }
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
