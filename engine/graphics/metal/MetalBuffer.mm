// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <algorithm>
#include "MetalBuffer.hpp"
#include "MetalError.hpp"
#include "MetalRenderDevice.hpp"

namespace ouzel::graphics::metal
{
    Buffer::Buffer(RenderDevice& initRenderDevice,
                   BufferType initType,
                   Flags initFlags,
                   const std::vector<std::uint8_t>& data,
                   std::uint32_t initSize):
        RenderResource(initRenderDevice),
        type(initType),
        flags(initFlags)
    {
        createBuffer(initSize);

        if (!data.empty())
            std::copy(data.begin(), data.end(), static_cast<std::uint8_t*>([buffer.get() contents]));
    }

    void Buffer::setData(const std::vector<std::uint8_t>& data)
    {
        if ((flags & Flags::dynamic) != Flags::dynamic)
            throw Error("Buffer is not dynamic");

        if (data.empty())
            throw Error("Data is empty");

        if (!buffer || data.size() > size)
            createBuffer(static_cast<std::uint32_t>(data.size()));

        std::copy(data.begin(), data.end(), static_cast<std::uint8_t*>([buffer.get() contents]));
    }

    void Buffer::createBuffer(NSUInteger newSize)
    {
        if (newSize > 0)
        {
            size = newSize;

            buffer = [renderDevice.getDevice().get() newBufferWithLength:size
                                                                 options:MTLResourceCPUCacheModeWriteCombined];

            if (!buffer)
                throw Error("Failed to create Metal buffer");
        }
    }
}

#endif
