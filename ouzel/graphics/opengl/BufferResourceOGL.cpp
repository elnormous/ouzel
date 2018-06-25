// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "BufferResourceOGL.hpp"
#include "RenderDeviceOGL.hpp"
#include "utils/Errors.hpp"

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

        void BufferResourceOGL::init(Buffer::Usage newUsage, uint32_t newFlags,
                                     const std::vector<uint8_t>& newData,
                                     uint32_t newSize)
        {
            BufferResource::init(newUsage, newFlags, newData, newSize);

            createBuffer();

            bufferSize = static_cast<GLsizeiptr>(newSize);

            if (bufferSize > 0)
            {
                if (!renderDeviceOGL.bindBuffer(bufferType, bufferId))
                    throw DataError("Failed to bind buffer");

                if (data.empty())
                    glBufferDataProc(bufferType, bufferSize, nullptr,
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                else
                    glBufferDataProc(bufferType, bufferSize, data.data(),
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                if (RenderDeviceOGL::checkOpenGLError())
                    throw DataError("Failed to create buffer");
            }
        }

        void BufferResourceOGL::reload()
        {
            bufferId = 0;

            createBuffer();

            if (bufferSize > 0)
            {
                if (!renderDeviceOGL.bindBuffer(bufferType, bufferId))
                    throw DataError("Failed to bind buffer");

                if (data.empty())
                    glBufferDataProc(bufferType, bufferSize, nullptr,
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                else
                    glBufferDataProc(bufferType, bufferSize, data.data(),
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                if (RenderDeviceOGL::checkOpenGLError())
                    throw DataError("Failed to create buffer");
            }
        }

        void BufferResourceOGL::setData(const std::vector<uint8_t>& newData)
        {
            BufferResource::setData(newData);

            if (!bufferId)
                throw DataError("Buffer not initialized");

            if (!renderDeviceOGL.bindBuffer(bufferType, bufferId))
                throw DataError("Failed to bind buffer");

            if (static_cast<GLsizeiptr>(data.size()) > bufferSize)
            {
                bufferSize = static_cast<GLsizeiptr>(data.size());

                glBufferDataProc(bufferType, bufferSize, data.data(),
                                 (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                if (RenderDeviceOGL::checkOpenGLError())
                    throw DataError("Failed to create buffer");
            }
            else
            {
                glBufferSubDataProc(bufferType, 0, static_cast<GLsizeiptr>(data.size()), data.data());

                if (RenderDeviceOGL::checkOpenGLError())
                    throw DataError("Failed to upload buffer");
            }
        }

        void BufferResourceOGL::createBuffer()
        {
            if (bufferId) renderDeviceOGL.deleteBuffer(bufferId);

            glGenBuffersProc(1, &bufferId);

            if (RenderDeviceOGL::checkOpenGLError())
                throw DataError("Failed to create buffer");

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
                    throw DataError("Unsupported buffer type");
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
