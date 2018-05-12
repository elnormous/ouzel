// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "BufferResourceOGL.hpp"
#include "RenderDeviceOGL.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        BufferResourceOGL::BufferResourceOGL(RenderDeviceOGL& initRenderDeviceOGL):
            renderDeviceOGL(initRenderDeviceOGL)
        {
        }

        BufferResourceOGL::~BufferResourceOGL()
        {
            if (bufferId)
                renderDeviceOGL.deleteBuffer(bufferId);
        }

        bool BufferResourceOGL::init(Buffer::Usage newUsage, uint32_t newFlags,
                                     const std::vector<uint8_t>& newData,
                                     uint32_t newSize)
        {
            if (!BufferResource::init(newUsage, newFlags, newData, newSize))
                return false;

            if (!createBuffer())
                return false;

            bufferSize = static_cast<GLsizeiptr>(newSize);

            if (bufferSize > 0)
            {
                renderDeviceOGL.bindVertexArray(0);

                if (!renderDeviceOGL.bindBuffer(bufferType, bufferId))
                    return false;

                if (data.empty())
                    glBufferDataProc(bufferType, bufferSize, nullptr,
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                else
                    glBufferDataProc(bufferType, bufferSize, data.data(),
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                if (RenderDeviceOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to create buffer";
                    return false;
                }
            }

            return true;
        }

        bool BufferResourceOGL::reload()
        {
            bufferId = 0;

            if (!createBuffer())
                return false;

            if (bufferSize > 0)
            {
                renderDeviceOGL.bindVertexArray(0);

                if (!renderDeviceOGL.bindBuffer(bufferType, bufferId))
                    return false;

                if (data.empty())
                    glBufferDataProc(bufferType, bufferSize, nullptr,
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                else
                    glBufferDataProc(bufferType, bufferSize, data.data(),
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                if (RenderDeviceOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to create buffer";
                    return false;
                }
            }

            return true;
        }

        bool BufferResourceOGL::setData(const std::vector<uint8_t>& newData)
        {
            if (!BufferResource::setData(newData))
                return false;

            if (!bufferId)
            {
                Log(Log::Level::ERR) << "Buffer not initialized";
                return false;
            }

            renderDeviceOGL.bindVertexArray(0);

            if (!renderDeviceOGL.bindBuffer(bufferType, bufferId))
                return false;

            if (static_cast<GLsizeiptr>(data.size()) > bufferSize)
            {
                bufferSize = static_cast<GLsizeiptr>(data.size());

                glBufferDataProc(bufferType, bufferSize, data.data(),
                                 (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                if (RenderDeviceOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to create buffer";
                    return false;
                }
            }
            else
            {
                glBufferSubDataProc(bufferType, 0, static_cast<GLsizeiptr>(data.size()), data.data());

                if (RenderDeviceOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to upload buffer";
                    return false;
                }
            }

            return true;
        }

        bool BufferResourceOGL::createBuffer()
        {
            if (bufferId) renderDeviceOGL.deleteBuffer(bufferId);

            glGenBuffersProc(1, &bufferId);

            if (RenderDeviceOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to create buffer";
                return false;
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
    } // namespace graphics
} // namespace ouzel

#endif
