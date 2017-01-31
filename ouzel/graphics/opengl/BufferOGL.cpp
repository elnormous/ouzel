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
                RendererOGL::deleteBuffer(bufferId);
            }
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

            if (data.dirty)
            {
                if (!bufferId)
                {
                    glGenBuffers(1, &bufferId);
                }

                switch (data.usage)
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

                if (!data.data.empty())
                {
                    RendererOGL::bindVertexArray(0);

                    if (!RendererOGL::bindBuffer(bufferType, bufferId))
                    {
                        return false;
                    }

                    if (static_cast<GLsizeiptr>(data.data.size()) > bufferSize)
                    {
                        bufferSize = static_cast<GLsizeiptr>(data.data.size());

                        glBufferData(bufferType, bufferSize, nullptr,
                                     data.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                        if (RendererOGL::checkOpenGLError())
                        {
                            Log(Log::Level::ERR) << "Failed to create buffer";
                            return false;
                        }
                    }

                    void* bufferPtr = nullptr;

    #if OUZEL_OPENGL_INTERFACE_EGL
                    if (glMapBufferRangeProc)
                    {
                        bufferPtr = glMapBufferRangeProc(bufferType, 0, static_cast<GLsizeiptr>(data.data.size()), GL_MAP_UNSYNCHRONIZED_BIT_EXT | GL_MAP_WRITE_BIT_EXT);
                    }
                    else if (glMapBufferProc)
                    {
                        bufferPtr = glMapBufferProc(bufferType, GL_WRITE_ONLY_OES);
                    }
    #else
                    bufferPtr = glMapBufferRange(bufferType, 0, static_cast<GLsizeiptr>(data.data.size()), GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT);
    #endif

                    if (bufferPtr)
                    {
                        std::copy(data.data.begin(), data.data.end(), static_cast<uint8_t*>(bufferPtr));

    #if OUZEL_OPENGL_INTERFACE_EGL
                        if (glUnmapBufferProc) glUnmapBufferProc(bufferType);
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

                        glBufferData(bufferType, bufferSize, data.data.data(),
                                     data.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                        if (RendererOGL::checkOpenGLError())
                        {
                            Log(Log::Level::ERR) << "Failed to upload buffer";
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
