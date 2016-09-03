// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferMetal.h"
#include "core/Engine.h"
#include "RendererMetal.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferMetal::MeshBufferMetal()
        {
        }

        MeshBufferMetal::~MeshBufferMetal()
        {
            if (indexBuffer)
            {
                [indexBuffer release];
            }

            if (vertexBuffer)
            {
                [vertexBuffer release];
            }
        }

        void MeshBufferMetal::free()
        {
            MeshBuffer::free();

            if (indexBuffer)
            {
                [indexBuffer release];
                indexBuffer = Nil;
            }

            if (vertexBuffer)
            {
                [vertexBuffer release];
                vertexBuffer = Nil;
            }
        }

        bool MeshBufferMetal::uploadBuffer(MTLBufferPtr buffer, const std::vector<uint8_t>& data)
        {
            memcpy([buffer contents], data.data(), data.size());

            return true;
        }

        bool MeshBufferMetal::upload()
        {
            if (dirty)
            {
                std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

                if (dirty & INDEX_SIZE_DIRTY)
                {
                    switch (uploadData.indexSize)
                    {
                        case 2:
                            indexFormat = MTLIndexTypeUInt16;
                            bytesPerIndex = 2;
                            break;
                        case 4:
                            indexFormat = MTLIndexTypeUInt32;
                            bytesPerIndex = 4;
                            break;
                        default:
                            bytesPerIndex = 0;
                            log("Invalid index size");
                            return false;
                    }

                    dirty &= ~INDEX_SIZE_DIRTY;
                }

                if (dirty & INDEX_BUFFER_DIRTY)
                {
                    if (!uploadData.indexData.empty())
                    {
                        if (!indexBuffer || uploadData.indexData.size() > indexBufferSize)
                        {
                            if (indexBuffer) [indexBuffer release];

                            indexBufferSize = static_cast<uint32_t>(uploadData.indexData.size());

                            indexBuffer = [rendererMetal->getDevice() newBufferWithLength:indexBufferSize
                                                                                  options:MTLResourceCPUCacheModeWriteCombined];

                            if (!indexBuffer)
                            {
                                log("Failed to create Metal index buffer");
                                return false;
                            }
                        }

                        if (!uploadBuffer(indexBuffer, uploadData.indexData))
                        {
                            return false;
                        }
                    }

                    dirty &= ~INDEX_BUFFER_DIRTY;
                }

                if (dirty & VERTEX_BUFFER_DIRTY)
                {
                    if (!uploadData.vertexData.empty())
                    {
                        if (!vertexBuffer || uploadData.vertexData.size() > vertexBufferSize)
                        {
                            if (vertexBuffer) [vertexBuffer release];

                            vertexBufferSize = static_cast<uint32_t>(uploadData.vertexData.size());

                            vertexBuffer = [rendererMetal->getDevice() newBufferWithLength:vertexBufferSize
                                                                                   options:MTLResourceCPUCacheModeWriteCombined];

                            if (!vertexBuffer)
                            {
                                log("Failed to create Metal vertex buffer");
                                return false;
                            }
                        }

                        if (!uploadBuffer(vertexBuffer, uploadData.vertexData))
                        {
                            return false;
                        }
                    }

                    dirty &= ~VERTEX_BUFFER_DIRTY;
                }

                dirty = 0;
                ready = (indexBuffer && vertexBuffer);
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
