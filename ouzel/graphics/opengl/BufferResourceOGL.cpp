// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "BufferResourceOGL.hpp"
#include "RenderDeviceOGL.hpp"

namespace ouzel
{
    namespace graphics
    {
        BufferResourceOGL::BufferResourceOGL(RenderDeviceOGL& renderDeviceOGL,
                                             Buffer::Usage newUsage, uint32_t newFlags,
                                             const std::vector<uint8_t>& newData,
                                             uint32_t newSize):
            RenderResourceOGL(renderDeviceOGL),
            usage(newUsage),
            flags(newFlags),
            data(newData)
        {
            createBuffer();

            size = static_cast<GLsizeiptr>(newSize);

            if (size > 0)
            {
                renderDeviceOGL.bindBuffer(bufferType, bufferId);

                if (data.empty())
                    glBufferDataProc(bufferType, size, nullptr,
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                else
                    glBufferDataProc(bufferType, size, data.data(),
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                GLenum error;

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(static_cast<int>(error), openGLErrorCategory, "Failed to create buffer");
            }
        }

        BufferResourceOGL::~BufferResourceOGL()
        {
            if (bufferId)
                renderDevice.deleteBuffer(bufferId);
        }

        void BufferResourceOGL::reload()
        {
            bufferId = 0;

            createBuffer();

            if (size > 0)
            {
                renderDevice.bindBuffer(bufferType, bufferId);

                if (data.empty())
                    glBufferDataProc(bufferType, size, nullptr,
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                else
                    glBufferDataProc(bufferType, size, data.data(),
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                GLenum error;

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(static_cast<int>(error), openGLErrorCategory, "Failed to create buffer");
            }
        }

        void BufferResourceOGL::setData(const std::vector<uint8_t>& newData)
        {
            if (!(flags & Buffer::DYNAMIC))
                throw std::runtime_error("Buffer is not dynamic");

            if (newData.empty())
                throw std::invalid_argument("Data is empty");

            data = newData;

            if (!bufferId)
                throw std::runtime_error("Buffer not initialized");

            renderDevice.bindBuffer(bufferType, bufferId);

            if (static_cast<GLsizeiptr>(data.size()) > size)
            {
                size = static_cast<GLsizeiptr>(data.size());

                glBufferDataProc(bufferType, size, data.data(),
                                 (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                GLenum error;

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(static_cast<int>(error), openGLErrorCategory, "Failed to create buffer");
            }
            else
            {
                glBufferSubDataProc(bufferType, 0, static_cast<GLsizeiptr>(data.size()), data.data());

                GLenum error;

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(static_cast<int>(error), openGLErrorCategory, "Failed to upload buffer");
            }
        }

        void BufferResourceOGL::createBuffer()
        {
            if (bufferId) renderDevice.deleteBuffer(bufferId);

            glGenBuffersProc(1, &bufferId);

            GLenum error;

            if ((error = glGetErrorProc()) != GL_NO_ERROR)
                throw std::system_error(static_cast<int>(error), openGLErrorCategory, "Failed to create buffer");

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
                    throw std::runtime_error("Unsupported buffer type");
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
