// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <algorithm>
#include "BufferResourceMetal.hpp"
#include "RenderDeviceMetal.hpp"
#include "utils/Log.hpp"

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

        bool BufferResourceMetal::init(Buffer::Usage newUsage, uint32_t newFlags,
                                       const std::vector<uint8_t>& newData,
                                       uint32_t newSize)
        {
            if (!BufferResource::init(newUsage, newFlags, newData, newSize))
                return false;

            if (!createBuffer(newSize))
                return false;

            if (!data.empty())
                std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));

            return true;
        }

        bool BufferResourceMetal::setData(const std::vector<uint8_t>& newData)
        {
            if (!BufferResource::setData(newData))
                return false;

            if (!buffer || data.size() > bufferSize)
            {
                if (!createBuffer(static_cast<uint32_t>(data.size())))
                    return false;
            }

            if (!data.empty())
                std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));

            return true;
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
                {
                    Log(Log::Level::ERR) << "Failed to create Metal buffer";
                    return false;
                }
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
