// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "VertexBufferMetal.h"
#include "RendererMetal.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        VertexBufferMetal::VertexBufferMetal()
        {
        }

        VertexBufferMetal::~VertexBufferMetal()
        {
            if (buffer)
            {
                [buffer release];
            }
        }

        void VertexBufferMetal::free()
        {
            VertexBuffer::free();

            if (buffer)
            {
                [buffer release];
                buffer = Nil;
            }

            bufferSize = 0;
        }

        bool VertexBufferMetal::upload()
        {
            if (uploadData.dirty)
            {
                RendererMetal* rendererMetal = static_cast<RendererMetal*>(sharedEngine->getRenderer());

                if (uploadData.dirty & VERTEX_BUFFER_DIRTY)
                {
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
                                Log(Log::Level::ERR) << "Failed to create Metal vertex buffer";
                                return false;
                            }
                        }

                        std::copy(uploadData.data.begin(), uploadData.data.end(), static_cast<uint8_t*>([buffer contents]));
                    }

                    uploadData.dirty &= ~VERTEX_BUFFER_DIRTY;
                }

                uploadData.dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
