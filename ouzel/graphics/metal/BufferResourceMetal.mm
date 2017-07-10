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

        bool BufferResourceMetal::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
                RendererMetal* rendererMetal = static_cast<RendererMetal*>(sharedEngine->getRenderer());

                if (!data.empty())
                {
                    if (!buffer || data.size() > bufferSize)
                    {
                        if (buffer) [buffer release];

                        bufferSize = static_cast<uint32_t>(data.size());

                        buffer = [rendererMetal->getDevice() newBufferWithLength:bufferSize
                                                                         options:MTLResourceCPUCacheModeWriteCombined];

                        if (!buffer)
                        {
                            Log(Log::Level::ERR) << "Failed to create Metal buffer";
                            return false;
                        }
                    }

                    std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));
                }

                dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
