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
        MeshBufferMetal::MeshBufferMetal():
            indexBufferDirty(false), vertexBufferDirty(false)
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
            std::lock_guard<std::mutex> lock(dataMutex);

            MeshBuffer::free();

            indexData.clear();
            vertexData.clear();

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

        bool MeshBufferMetal::init(bool newDynamicIndexBuffer, bool newDynamicVertexBuffer)
        {
            free();

            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::init(newDynamicIndexBuffer, newDynamicVertexBuffer))
            {
                return false;
            }

            indexBufferDirty = true;
            vertexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());
            
            return true;
        }

        bool MeshBufferMetal::initFromBuffer(const void* newIndices, uint32_t newIndexSize,
                                             uint32_t newIndexCount, bool newDynamicIndexBuffer,
                                             const void* newVertices, uint32_t newVertexAttributes,
                                             uint32_t newVertexCount, bool newDynamicVertexBuffer)
        {
            free();
            
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::initFromBuffer(newIndices, newIndexSize, newIndexCount, newDynamicIndexBuffer, newVertices, newVertexAttributes, newVertexCount, newDynamicVertexBuffer))
            {
                return false;
            }

            indexData.assign(static_cast<const uint8_t*>(newIndices),
                             static_cast<const uint8_t*>(newIndices) + indexSize * indexCount);


            vertexData.assign(static_cast<const uint8_t*>(newVertices),
                              static_cast<const uint8_t*>(newVertices) + vertexSize * vertexCount);

            indexBufferDirty = true;
            vertexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferMetal::setIndexSize(uint32_t indexSize)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::setIndexSize(indexSize))
            {
                return false;
            }

            indexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferMetal::setVertexAttributes(uint32_t vertexAttributes)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::setVertexAttributes(vertexAttributes))
            {
                return false;
            }

            vertexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferMetal::uploadIndices(const void* newIndices, uint32_t newIndexCount)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::uploadIndices(newIndices, newIndexCount))
            {
                return false;
            }

            indexData.assign(static_cast<const uint8_t*>(newIndices),
                             static_cast<const uint8_t*>(newIndices) + indexSize * indexCount);

            indexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferMetal::uploadVertices(const void* newVertices, uint32_t newVertexCount)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::uploadVertices(newVertices, newVertexCount))
            {
                return false;
            }

            vertexData.assign(static_cast<const uint8_t*>(newVertices),
                              static_cast<const uint8_t*>(newVertices) + vertexSize * vertexCount);

            vertexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferMetal::uploadData(MTLBufferPtr buffer, const std::vector<uint8_t>& data)
        {
            memcpy([buffer contents], data.data(), data.size());

            return true;
        }

        bool MeshBufferMetal::update()
        {
            if (indexBufferDirty || vertexBufferDirty)
            {
                std::vector<uint8_t> localIndexData;
                std::vector<uint8_t> localVertexData;

                {
                    std::lock_guard<std::mutex> lock(dataMutex);

                    if (indexBufferDirty)
                    {
                        localIndexData = indexData;
                        switch (indexSize)
                        {
                            case 2: indexFormat = MTLIndexTypeUInt16; break;
                            case 4: indexFormat = MTLIndexTypeUInt32; break;
                            default: log("Invalid index size"); return false;
                        }
                    }

                    if (vertexBufferDirty)
                    {
                        localVertexData = vertexData;
                    }
                }

                std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

                if (indexBufferDirty)
                {
                    if (!localIndexData.empty())
                    {
                        if (!indexBuffer || localIndexData.size() > indexBufferSize)
                        {
                            if (indexBuffer) [indexBuffer release];

                            indexBufferSize = localIndexData.size();

                            indexBuffer = [rendererMetal->getDevice() newBufferWithLength:indexBufferSize
                                                                                  options:MTLResourceCPUCacheModeWriteCombined];

                            if (!indexBuffer)
                            {
                                log("Failed to create Metal index buffer");
                                return false;
                            }
                        }

                        if (!uploadData(indexBuffer, localIndexData))
                        {
                            return false;
                        }
                    }

                    indexBufferDirty = false;
                }

                if (vertexBufferDirty)
                {
                    if (!localVertexData.empty())
                    {
                        if (!vertexBuffer || localVertexData.size() > vertexBufferSize)
                        {
                            if (vertexBuffer) [vertexBuffer release];

                            vertexBufferSize = localVertexData.size();

                            vertexBuffer = [rendererMetal->getDevice() newBufferWithLength:vertexBufferSize
                                                                                   options:MTLResourceCPUCacheModeWriteCombined];
                            
                            if (!vertexBuffer)
                            {
                                log("Failed to create Metal vertex buffer");
                                return false;
                            }
                        }
                        
                        if (!uploadData(vertexBuffer, localVertexData))
                        {
                            return false;
                        }
                    }
                    
                    vertexBufferDirty = false;
                }

                ready = (indexBuffer && vertexBuffer);
            }
            
            return true;
        }
    } // namespace graphics
} // namespace ouzel
