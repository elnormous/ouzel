// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "IndexBufferMetal.h"
#include "RendererMetal.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        IndexBufferMetal::IndexBufferMetal()
        {
        }

        IndexBufferMetal::~IndexBufferMetal()
        {
            if (buffer)
            {
                [buffer release];
            }
        }

        void IndexBufferMetal::free()
        {
            IndexBufferResource::free();

            if (buffer)
            {
                [buffer release];
                buffer = Nil;
            }

            bufferSize = 0;
        }

        bool IndexBufferMetal::upload()
        {
            if (!IndexBufferResource::upload())
            {
                return false;
            }

            RendererMetal* rendererMetal = static_cast<RendererMetal*>(sharedEngine->getRenderer());

            if (!uploadData.data.empty())
            {
                if (!buffer || uploadData.data.size() > bufferSize)
                {
                    if (buffer) [buffer release];

                    bufferSize = static_cast<uint32_t>(uploadData.data.size());

                    buffer = [rendererMetal->getDevice() newBufferWithLength:bufferSize
                                                                     options:MTLResourceCPUCacheModeWriteCombined];

                    if (!buffer)
                    {
                        Log(Log::Level::ERR) << "Failed to create Metal index buffer";
                        return false;
                    }
                }

                std::copy(uploadData.data.begin(), uploadData.data.end(), static_cast<uint8_t*>([buffer contents]));
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
