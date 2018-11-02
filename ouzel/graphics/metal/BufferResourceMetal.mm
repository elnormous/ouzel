// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <algorithm>
#include "BufferResourceMetal.hpp"
#include "RenderDeviceMetal.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace graphics
    {
        BufferResourceMetal::BufferResourceMetal(RenderDeviceMetal& renderDeviceMetal,
                                                 Buffer::Usage newUsage, uint32_t newFlags,
                                                 const std::vector<uint8_t>& data,
                                                 uint32_t newSize):
            RenderResourceMetal(renderDeviceMetal),
            usage(newUsage),
            flags(newFlags)
        {
            createBuffer(newSize);

            if (!data.empty())
                std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));
        }

        BufferResourceMetal::~BufferResourceMetal()
        {
            if (buffer) [buffer release];
        }

        void BufferResourceMetal::setData(const std::vector<uint8_t>& data)
        {
            if (!(flags & Buffer::DYNAMIC))
                throw DataError("Buffer is not dynamic");

            if (data.empty())
                throw DataError("Data is empty");

            if (!buffer || data.size() > size)
                createBuffer(static_cast<uint32_t>(data.size()));

            if (!data.empty())
                std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));
        }

        void BufferResourceMetal::createBuffer(NSUInteger newSize)
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
                    throw DataError("Failed to create Metal buffer");
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
