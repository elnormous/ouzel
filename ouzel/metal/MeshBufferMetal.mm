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
            indexBufferDirty(true), vertexBufferDirty(true)
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

        bool MeshBufferMetal::initFromBuffer(const void* newIndices, uint32_t newIndexSize,
                                             uint32_t newIndexCount, bool newDynamicIndexBuffer,
                                             const void* newVertices, uint32_t newVertexAttributes,
                                             uint32_t newVertexCount, bool newDynamicVertexBuffer)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::initFromBuffer(newIndices, newIndexSize, newIndexCount, newDynamicIndexBuffer, newVertices, newVertexAttributes, newVertexCount, newDynamicVertexBuffer))
            {
                return false;
            }

            if (!updateIndexFormat())
            {
                return false;
            }

            indexData.assign(static_cast<const uint8_t*>(newIndices),
                             static_cast<const uint8_t*>(newIndices) + indexSize * indexCount);


            vertexData.assign(static_cast<const uint8_t*>(newVertices),
                              static_cast<const uint8_t*>(newVertices) + vertexSize * vertexCount);

            ready = true;

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

            return updateIndexFormat();
        }

        bool MeshBufferMetal::setVertexAttributes(uint32_t vertexAttributes)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::setVertexAttributes(vertexAttributes))
            {
                return false;
            }

            return true;
        }

        bool MeshBufferMetal::uploadIndices(const void* indices, uint32_t indexCount)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::uploadIndices(indices, indexCount))
            {
                return false;
            }

            indexData.assign(static_cast<const uint8_t*>(indices),
                             static_cast<const uint8_t*>(indices) + indexSize * indexCount);

            indexBufferDirty = true;

            return true;
        }

        bool MeshBufferMetal::uploadVertices(const void* vertices, uint32_t vertexCount)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::uploadVertices(vertices, vertexCount))
            {
                return false;
            }

            vertexData.assign(static_cast<const uint8_t*>(vertices),
                              static_cast<const uint8_t*>(vertices) + vertexSize * vertexCount);

            vertexBufferDirty = true;

            return true;
        }

        bool MeshBufferMetal::updateIndexFormat()
        {
            switch (indexSize)
            {
                case 2: indexFormat = MTLIndexTypeUInt16; break;
                case 4: indexFormat = MTLIndexTypeUInt32; break;
                default: log("Invalid index size"); return false;
            }

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
                std::lock_guard<std::mutex> lock(dataMutex);

                std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

                if (indexBufferDirty)
                {
                    if (!indexBuffer || indexSize * indexCount > indexBufferSize)
                    {
                        if (indexBuffer) [indexBuffer release];

                        indexBufferSize = indexSize * indexCount;

                        indexBuffer = [rendererMetal->getDevice() newBufferWithLength:indexBufferSize
                                                                              options:MTLResourceCPUCacheModeWriteCombined];

                        if (!indexBuffer)
                        {
                            log("Failed to create Metal index buffer");
                            return false;
                        }
                    }

                    if (!uploadData(indexBuffer, indexData))
                    {
                        return false;
                    }

                    indexBufferDirty = false;
                }

                if (vertexBufferDirty)
                {
                    if (!vertexBuffer || vertexSize * vertexCount > vertexBufferSize)
                    {
                        if (vertexBuffer) [vertexBuffer release];

                        vertexBufferSize = vertexSize * vertexCount;

                        vertexBuffer = [rendererMetal->getDevice() newBufferWithLength:vertexBufferSize
                                                                               options:MTLResourceCPUCacheModeWriteCombined];

                        if (!vertexBuffer)
                        {
                            log("Failed to create Metal vertex buffer");
                            return false;
                        }
                    }

                    if (!uploadData(vertexBuffer, vertexData))
                    {
                        return false;
                    }

                    vertexBufferDirty = false;
                }
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
