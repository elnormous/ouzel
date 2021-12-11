// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGLBuffer.hpp"
#include "OGLError.hpp"
#include "OGLRenderDevice.hpp"

namespace ouzel::graphics::opengl
{
    Buffer::Buffer(RenderDevice& initRenderDevice,
                   BufferType initType,
                   Flags initFlags,
                   const std::vector<std::uint8_t>& initData,
                   std::uint32_t initSize):
        RenderResource{initRenderDevice},
        type{initType},
        flags{initFlags},
        data{initData},
        size{static_cast<GLsizeiptr>(initSize)}
    {
        createBuffer();

        if (size > 0)
        {
            renderDevice.bindBuffer(bufferType, bufferId);

            if (data.empty())
                renderDevice.glBufferDataProc(bufferType, size, nullptr,
                                              (flags & Flags::dynamic) == Flags::dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            else
                renderDevice.glBufferDataProc(bufferType, size, data.data(),
                                              (flags & Flags::dynamic) == Flags::dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error{makeErrorCode(error), "Failed to create buffer"};
        }
    }

    Buffer::~Buffer()
    {
        if (bufferId)
            renderDevice.deleteBuffer(bufferId);
    }

    void Buffer::reload()
    {
        bufferId = 0;

        createBuffer();

        if (size > 0)
        {
            renderDevice.bindBuffer(bufferType, bufferId);

            if (data.empty())
                renderDevice.glBufferDataProc(bufferType, size, nullptr,
                                              (flags & Flags::dynamic) == Flags::dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            else
                renderDevice.glBufferDataProc(bufferType, size, data.data(),
                                              (flags & Flags::dynamic) == Flags::dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error{makeErrorCode(error), "Failed to create buffer"};
        }
    }

    void Buffer::setData(const std::vector<std::uint8_t>& newData)
    {
        if ((flags & Flags::dynamic) != Flags::dynamic)
            throw Error("Buffer is not dynamic");

        if (newData.empty())
            throw std::invalid_argument("Data is empty");

        data = newData;

        if (!bufferId)
            throw Error("Buffer not initialized");

        renderDevice.bindBuffer(bufferType, bufferId);

        if (static_cast<GLsizeiptr>(data.size()) > size)
        {
            size = static_cast<GLsizeiptr>(data.size());

            renderDevice.glBufferDataProc(bufferType, size, data.data(), GL_DYNAMIC_DRAW);

            if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error{makeErrorCode(error), "Failed to create buffer"};
        }
        else
        {
            renderDevice.glBufferSubDataProc(bufferType, 0, static_cast<GLsizeiptr>(data.size()), data.data());

            if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error{makeErrorCode(error), "Failed to upload buffer"};
        }
    }

    void Buffer::createBuffer()
    {
        renderDevice.glGenBuffersProc(1, &bufferId);

        if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error{makeErrorCode(error), "Failed to create buffer"};

        switch (type)
        {
            case BufferType::index:
                bufferType = GL_ELEMENT_ARRAY_BUFFER;
                break;
            case BufferType::vertex:
                bufferType = GL_ARRAY_BUFFER;
                break;
            default:
                throw Error("Unsupported buffer type");
        }
    }
}

#endif
