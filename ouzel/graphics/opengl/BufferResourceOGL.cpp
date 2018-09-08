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
        BufferResourceOGL::BufferResourceOGL(RenderDeviceOGL& renderDeviceOGL):
            BufferResource(renderDeviceOGL)
        {
        }

        BufferResourceOGL::~BufferResourceOGL()
        {
            RenderDeviceOGL& renderDeviceOGL = static_cast<RenderDeviceOGL&>(renderDevice);

            if (bufferId)
                renderDeviceOGL.deleteBuffer(bufferId);
        }

        void BufferResourceOGL::init(Buffer::Usage newUsage, uint32_t newFlags,
                                     const std::vector<uint8_t>& newData,
                                     uint32_t newSize)
        {
            usage = newUsage;
            flags = newFlags;
            data = newData;

            createBuffer();

            bufferSize = static_cast<GLsizeiptr>(newSize);

            if (bufferSize > 0)
            {
                RenderDeviceOGL& renderDeviceOGL = static_cast<RenderDeviceOGL&>(renderDevice);

                renderDeviceOGL.bindBuffer(bufferType, bufferId);

                if (data.empty())
                    glBufferDataProc(bufferType, bufferSize, nullptr,
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                else
                    glBufferDataProc(bufferType, bufferSize, data.data(),
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                GLenum error;

                if ((error = glGetError()) != GL_NO_ERROR)
                    throw DataError("Failed to create buffer, error: " + std::to_string(error));
            }
        }

        void BufferResourceOGL::reload()
        {
            bufferId = 0;

            createBuffer();

            if (bufferSize > 0)
            {
                RenderDeviceOGL& renderDeviceOGL = static_cast<RenderDeviceOGL&>(renderDevice);

                renderDeviceOGL.bindBuffer(bufferType, bufferId);

                if (data.empty())
                    glBufferDataProc(bufferType, bufferSize, nullptr,
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                else
                    glBufferDataProc(bufferType, bufferSize, data.data(),
                                     (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                GLenum error;

                if ((error = glGetError()) != GL_NO_ERROR)
                    throw DataError("Failed to create buffer, error: " + std::to_string(error));
            }
        }

        void BufferResourceOGL::setData(const std::vector<uint8_t>& newData)
        {
            if (!(flags & Buffer::DYNAMIC))
                throw DataError("Buffer is not dynamic");

            if (newData.empty())
                throw DataError("Data is empty");

            data = newData;

            if (!bufferId)
                throw DataError("Buffer not initialized");

            RenderDeviceOGL& renderDeviceOGL = static_cast<RenderDeviceOGL&>(renderDevice);
            renderDeviceOGL.bindBuffer(bufferType, bufferId);

            if (static_cast<GLsizeiptr>(data.size()) > bufferSize)
            {
                bufferSize = static_cast<GLsizeiptr>(data.size());

                glBufferDataProc(bufferType, bufferSize, data.data(),
                                 (flags & Texture::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                GLenum error;

                if ((error = glGetError()) != GL_NO_ERROR)
                    throw DataError("Failed to create buffer, error: " + std::to_string(error));
            }
            else
            {
                glBufferSubDataProc(bufferType, 0, static_cast<GLsizeiptr>(data.size()), data.data());

                GLenum error;

                if ((error = glGetError()) != GL_NO_ERROR)
                    throw DataError("Failed to upload buffer, error: " + std::to_string(error));
            }
        }

        void BufferResourceOGL::createBuffer()
        {
            RenderDeviceOGL& renderDeviceOGL = static_cast<RenderDeviceOGL&>(renderDevice);
            if (bufferId) renderDeviceOGL.deleteBuffer(bufferId);

            glGenBuffersProc(1, &bufferId);

            GLenum error;

            if ((error = glGetError()) != GL_NO_ERROR)
                throw DataError("Failed to create buffer, error: " + std::to_string(error));

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
