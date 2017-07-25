// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#include <algorithm>
#include "BufferResourceMetal.h"
#include "RendererMetal.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        BufferResourceMetal::BufferResourceMetal()
        {
        }

        BufferResourceMetal::~BufferResourceMetal()
        {
            if (buffer)
            {
                [buffer release];
            }
        }

        bool BufferResourceMetal::init(Buffer::Usage newUsage, bool newDynamic)
        {
            if (!BufferResource::init(newUsage, newDynamic))
            {
                return false;
            }

            return true;
        }

        bool BufferResourceMetal::init(Buffer::Usage newUsage, const std::vector<uint8_t>& newData, bool newDynamic)
        {
            if (!BufferResource::init(newUsage, newData, newDynamic))
            {
                return false;
            }

            if (!data.empty())
            {
                RendererMetal* rendererMetal = static_cast<RendererMetal*>(sharedEngine->getRenderer());

                bufferSize = static_cast<uint32_t>(data.size());

                if (buffer) [buffer release];

                buffer = [rendererMetal->getDevice() newBufferWithLength:bufferSize
                                                                 options:MTLResourceCPUCacheModeWriteCombined];

                if (!buffer)
                {
                    Log(Log::Level::ERR) << "Failed to create Metal buffer";
                    return false;
                }

                std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));
            }

            return true;
        }

        bool BufferResourceMetal::setData(const std::vector<uint8_t>& newData)
        {
            if (!BufferResource::setData(newData))
            {
                return false;
            }

            if (!data.empty())
            {
                if (!buffer || data.size() > bufferSize)
                {
                    RendererMetal* rendererMetal = static_cast<RendererMetal*>(sharedEngine->getRenderer());

                    bufferSize = static_cast<uint32_t>(data.size());

                    if (buffer) [buffer release];

                    buffer = [rendererMetal->getDevice() newBufferWithLength:bufferSize
                                                                     options:MTLResourceCPUCacheModeWriteCombined];

                    if (!buffer)
                    {
                        Log(Log::Level::ERR) << "Failed to create Metal buffer";
                        return false;
                    }

                    bufferSize = static_cast<uint32_t>(data.size());
                }

                std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
