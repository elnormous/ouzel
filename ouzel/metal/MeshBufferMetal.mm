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
            if (!MeshBuffer::initFromBuffer(newIndices, newIndexSize, newIndexCount, newDynamicIndexBuffer, newVertices, newVertexAttributes, newVertexCount, newDynamicVertexBuffer))
            {
                return false;
            }

            free();

            if (!createIndexBuffer(newIndices, indexSize * indexCount))
            {
                return false;
            }

            if (!updateIndexFormat())
            {
                return false;
            }

            if (!createVertexBuffer(newVertices, vertexSize * vertexCount))
            {
                return false;
            }

            ready = true;

            return true;
        }

        bool MeshBufferMetal::setIndexSize(uint32_t indexSize)
        {
            if (!MeshBuffer::setIndexSize(indexSize))
            {
                return false;
            }

            return updateIndexFormat();
        }

        bool MeshBufferMetal::uploadIndices(const void* indices, uint32_t indexCount)
        {
            if (!MeshBuffer::uploadIndices(indices, indexCount))
            {
                return false;
            }

            if (indexSize * indexCount > indexBufferSize)
            {
                if (indexBuffer)
                {
                    [indexBuffer release];
                    indexBuffer = Nil;
                }

                ready = createIndexBuffer(indices, indexSize * indexCount);

                return ready;
            }
            else
            {
                return uploadData(indexBuffer, indices, indexSize * indexCount);
            }
        }

        bool MeshBufferMetal::uploadVertices(const void* vertices, uint32_t vertexCount)
        {
            if (!MeshBuffer::uploadVertices(vertices, vertexCount))
            {
                return false;
            }

            if (vertexSize * vertexCount > vertexBufferSize)
            {
                if (vertexBuffer)
                {
                    [vertexBuffer release];
                    vertexBuffer = Nil;
                }

                ready = createVertexBuffer(vertices, vertexSize * vertexCount);

                return ready;
            }
            else
            {
                return uploadData(vertexBuffer, vertices, vertexSize * vertexCount);
            }

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

        bool MeshBufferMetal::createIndexBuffer(const void* indices, uint32_t size)
        {
            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            indexBuffer = [rendererMetal->getDevice() newBufferWithLength:size
                                                                  options:MTLResourceCPUCacheModeWriteCombined];

            if (indexBuffer == Nil)
            {
                log("Failed to create Metal index buffer");
                return false;
            }

            indexBufferSize = size;

            return uploadData(indexBuffer, indices, size);
        }

        bool MeshBufferMetal::createVertexBuffer(const void* vertices, uint32_t size)
        {
            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            vertexBuffer = [rendererMetal->getDevice() newBufferWithLength:size
                                                                   options:MTLResourceCPUCacheModeWriteCombined];

            if (vertexBuffer == Nil)
            {
                log("Failed to create Metal vertex buffer");
                return false;
            }

            vertexBufferSize = size;

            return uploadData(vertexBuffer, vertices, size);
        }

        bool MeshBufferMetal::uploadData(MTLBufferPtr buffer, const void* data, uint32_t size)
        {
            memcpy([buffer contents], data, size);

            return true;
        }

        bool MeshBufferMetal::update()
        {
            return true;
        }
    } // namespace graphics
} // namespace ouzel
