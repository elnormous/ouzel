// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
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
            VertexBufferResource::free();

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

            return true;
        }

        bool VertexBufferOGL::upload()
        {
            if (!VertexBufferResource::upload())
            {
                return false;
            }

            if (!bufferId)
            {
                glGenBuffers(1, &bufferId);
            }

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

                void* bufferPtr = nullptr;

#if OUZEL_OPENGL_INTERFACE_EGL
#if defined(GL_EXT_map_buffer_range)
                if (mapBufferRangeEXT)
                {
                    bufferPtr = mapBufferRangeEXT(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(uploadData.data.size()), GL_MAP_UNSYNCHRONIZED_BIT_EXT | GL_MAP_WRITE_BIT_EXT);
                }
#endif
#if defined(GL_OES_mapbuffer)
                if (!bufferPtr && mapBufferOES)
                {
                    bufferPtr = mapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
                }
#else
                bufferPtr = nullptr;
#endif
#else
                bufferPtr = glMapBufferRange(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(uploadData.data.size()), GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT);
#endif

                if (bufferPtr)
                {
                    std::copy(uploadData.data.begin(), uploadData.data.end(), static_cast<uint8_t*>(bufferPtr));

#if OUZEL_OPENGL_INTERFACE_EGL
#if defined(GL_OES_mapbuffer)
                    if (unmapBufferOES) unmapBufferOES(GL_ARRAY_BUFFER);
#endif
#else
                    glUnmapBuffer(GL_ARRAY_BUFFER);
#endif

                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to upload vertex buffer";
                        return false;
                    }
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


            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
