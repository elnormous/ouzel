// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "IndexBufferMetal.h"
#include "RendererMetal.h"
#include "core/Engine.h"
#include "utils/Utils.h"

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
            IndexBuffer::free();

            if (buffer)
            {
                [buffer release];
                buffer = Nil;
            }
        }

        bool IndexBufferMetal::upload()
        {
            if (dirty)
            {
                std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

                if (dirty & INDEX_SIZE_DIRTY)
                {
                    switch (uploadData.indexSize)
                    {
                        case 2:
                            type = MTLIndexTypeUInt16;
                            bytesPerIndex = 2;
                            break;
                        case 4:
                            type = MTLIndexTypeUInt32;
                            bytesPerIndex = 4;
                            break;
                        default:
                            bytesPerIndex = 0;
                            log(LOG_LEVEL_ERROR, "Invalid index size");
                            return false;
                    }

                    dirty &= ~INDEX_SIZE_DIRTY;
                }

                if (dirty & INDEX_BUFFER_DIRTY)
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
                                log(LOG_LEVEL_ERROR, "Failed to create Metal index buffer");
                                return false;
                            }
                        }

                        memcpy([buffer contents], uploadData.data.data(), uploadData.data.size());
                    }

                    dirty &= ~INDEX_BUFFER_DIRTY;

                    ready = true;
                }

                dirty = 0;
            }
            
            return true;
        }
    }
}
