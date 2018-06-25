// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

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
        BufferResourceMetal::BufferResourceMetal(RenderDeviceMetal& initRenderDeviceMetal):
            renderDeviceMetal(initRenderDeviceMetal)
        {
        }

        BufferResourceMetal::~BufferResourceMetal()
        {
            if (buffer) [buffer release];
        }

        void BufferResourceMetal::init(Buffer::Usage newUsage, uint32_t newFlags,
                                       const std::vector<uint8_t>& newData,
                                       uint32_t newSize)
        {
            BufferResource::init(newUsage, newFlags, newData, newSize);

            createBuffer(newSize);

            if (!data.empty())
                std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));
        }

        void BufferResourceMetal::setData(const std::vector<uint8_t>& newData)
        {
            BufferResource::setData(newData);

            if (!buffer || data.size() > bufferSize)
                createBuffer(static_cast<uint32_t>(data.size()));

            if (!data.empty())
                std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));
        }

        bool BufferResourceMetal::createBuffer(NSUInteger newSize)
        {
            if (buffer)
            {
                [buffer release];
                buffer = nil;
            }

            if (newSize > 0)
            {
                bufferSize = newSize;

                buffer = [renderDeviceMetal.getDevice() newBufferWithLength:bufferSize
                                                                    options:MTLResourceCPUCacheModeWriteCombined];

                if (!buffer)
                    throw DataError("Failed to create Metal buffer");
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
