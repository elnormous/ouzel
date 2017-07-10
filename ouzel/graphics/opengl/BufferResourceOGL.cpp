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

        bool BufferResourceOGL::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
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

                dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
