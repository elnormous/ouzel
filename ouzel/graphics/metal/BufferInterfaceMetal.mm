// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#include <algorithm>
#include "BufferInterfaceMetal.hpp"
#include "RendererMetal.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        BufferInterfaceMetal::BufferInterfaceMetal(RendererMetal* aRendererMetal):
            rendererMetal(aRendererMetal)
        {
        }

        BufferInterfaceMetal::~BufferInterfaceMetal()
        {
            if (buffer)
            {
                [buffer release];
            }
        }

        bool BufferInterfaceMetal::init(Buffer::Usage newUsage, uint32_t newFlags, uint32_t newSize)
        {
            if (!BufferInterface::init(newUsage, newFlags, newSize))
            {
                return false;
            }

            if (!createBuffer())
            {
                return false;
            }

            return true;
        }

        bool BufferInterfaceMetal::init(Buffer::Usage newUsage, const std::vector<uint8_t>& newData, uint32_t newFlags)
        {
            if (!BufferInterface::init(newUsage, newData, newFlags))
            {
                return false;
            }

            if (!createBuffer())
            {
                return false;
            }

            if (!data.empty())
            {
                std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));
            }

            return true;
        }

        bool BufferInterfaceMetal::setData(const std::vector<uint8_t>& newData)
        {
            if (!BufferInterface::setData(newData))
            {
                return false;
            }

            if (!buffer || data.size() > bufferSize)
            {
                if (!createBuffer())
                {
                    return false;
                }
            }

            if (!data.empty())
            {
                std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));
            }

            return true;
        }

        bool BufferInterfaceMetal::createBuffer()
        {
            if (buffer)
            {
                [buffer release];
                buffer = Nil;
            }

            bufferSize = static_cast<uint32_t>(data.size());

            if (!data.empty())
            {
                buffer = [rendererMetal->getDevice() newBufferWithLength:bufferSize
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
