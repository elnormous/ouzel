// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#include <algorithm>
#include "BufferResourceOGL.h"
#include "RendererOGL.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        BufferResourceOGL::BufferResourceOGL()
        {
        }

        BufferResourceOGL::~BufferResourceOGL()
        {
            if (bufferId)
            {
                RendererOGL::deleteBuffer(bufferId);
            }
        }

        bool BufferResourceOGL::init(Buffer::Usage newUsage, bool newDynamic)
        {
            if (!BufferResource::init(newUsage, newDynamic))
            {
                return false;
            }

            if (!bufferId)
            {
                glGenBuffersProc(1, &bufferId);
            }

            switch (usage)
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

            return true;
        }

        bool BufferResourceOGL::init(Buffer::Usage newUsage, const std::vector<uint8_t>& newData, bool newDynamic)
        {
            if (!BufferResource::init(newUsage, newData, newDynamic))
            {
                return false;
            }

            if (!bufferId)
            {
                glGenBuffersProc(1, &bufferId);
            }

            switch (usage)
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

            if (!data.empty())
            {
                RendererOGL::bindVertexArray(0);

                if (!RendererOGL::bindBuffer(bufferType, bufferId))
                {
                    return false;
                }

                if (static_cast<GLsizeiptr>(data.size()) > bufferSize)
                {
                    bufferSize = static_cast<GLsizeiptr>(data.size());

                    glBufferDataProc(bufferType, bufferSize, data.data(),
                                     dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to create buffer";
                        return false;
                    }
                }
                else
                {
                    glBufferSubDataProc(bufferType, 0, static_cast<GLsizeiptr>(data.size()), data.data());

                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to upload buffer";
                        return false;
                    }
                }
            }

            return true;
        }

        bool BufferResourceOGL::setData(const std::vector<uint8_t>& newData)
        {
            if (!BufferResource::setData(newData))
            {
                return false;
            }

            if (!bufferId)
            {
                Log(Log::Level::ERR) << "Buffer not initialized";
                return false;
            }

            if (!data.empty())
            {
                RendererOGL::bindVertexArray(0);

                if (!RendererOGL::bindBuffer(bufferType, bufferId))
                {
                    return false;
                }

                if (static_cast<GLsizeiptr>(data.size()) > bufferSize)
                {
                    bufferSize = static_cast<GLsizeiptr>(data.size());

                    glBufferDataProc(bufferType, bufferSize, data.data(),
                                     dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to create buffer";
                        return false;
                    }
                }
                else
                {
                    glBufferSubDataProc(bufferType, 0, static_cast<GLsizeiptr>(data.size()), data.data());

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

#endif
