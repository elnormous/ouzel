// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <algorithm>
#include <stdexcept>
#include "MetalBuffer.hpp"
#include "MetalRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        MetalBuffer::MetalBuffer(MetalRenderDevice& renderDeviceMetal,
                                 Buffer::Usage newUsage, uint32_t newFlags,
                                 const std::vector<uint8_t>& data,
                                 uint32_t newSize):
            MetalRenderResource(renderDeviceMetal),
            usage(newUsage),
            flags(newFlags)
        {
            createBuffer(newSize);

            if (!data.empty())
                std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));
        }

        MetalBuffer::~MetalBuffer()
        {
            if (buffer) [buffer release];
        }

        void MetalBuffer::setData(const std::vector<uint8_t>& data)
        {
            if (!(flags & Buffer::DYNAMIC))
                throw std::runtime_error("Buffer is not dynamic");

            if (data.empty())
                throw std::runtime_error("Data is empty");

            if (!buffer || data.size() > size)
                createBuffer(static_cast<uint32_t>(data.size()));

            std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));
        }

        void MetalBuffer::createBuffer(NSUInteger newSize)
        {
            if (buffer)
            {
                [buffer release];
                buffer = nil;
            }

            if (newSize > 0)
            {
                size = newSize;

                buffer = [renderDevice.getDevice() newBufferWithLength:size
                                                               options:MTLResourceCPUCacheModeWriteCombined];

                if (!buffer)
                    throw std::runtime_error("Failed to create Metal buffer");
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
