// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

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
                std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

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

                        memcpy([buffer contents], uploadData.data.data(), uploadData.data.size());
                    }

                    uploadData.dirty &= ~VERTEX_BUFFER_DIRTY;
                }

                uploadData.dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
