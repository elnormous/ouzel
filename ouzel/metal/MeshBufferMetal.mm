// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferMetal.h"
#include "Engine.h"
#include "RendererMetal.h"
#include "Utils.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferMetal::MeshBufferMetal()
        {

        }

        MeshBufferMetal::~MeshBufferMetal()
        {
            destroy();
        }

        void MeshBufferMetal::destroy()
        {
            if (_indexBuffer)
            {
                [_indexBuffer release];
                _indexBuffer = Nil;
            }

            if (_vertexBuffer)
            {
                [_vertexBuffer release];
                _vertexBuffer = Nil;
            }
        }

        bool MeshBufferMetal::initFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer)
        {
            if (!MeshBuffer::initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexAttributes, vertexCount, dynamicVertexBuffer))
            {
                return false;
            }

            destroy();

            if (!createIndexBuffer(indices, _indexSize * indexCount))
            {
                return false;
            }

            if (!updateIndexFormat())
            {
                return false;
            }

            if (!createVertexBuffer(vertices, _vertexSize * vertexCount))
            {
                return false;
            }

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

            if (_indexSize * indexCount > _indexBufferSize)
            {
                if (_indexBuffer) [_indexBuffer release];
                return createIndexBuffer(indices, _indexSize * indexCount);
            }
            else
            {
                return uploadData(_indexBuffer, indices, _indexSize * indexCount);
            }
        }

        bool MeshBufferMetal::uploadVertices(const void* vertices, uint32_t vertexCount)
        {
            if (!MeshBuffer::uploadVertices(vertices, vertexCount))
            {
                return false;
            }

            if (_vertexSize * vertexCount > _vertexBufferSize)
            {
                if (_vertexBuffer) [_vertexBuffer release];
                return createVertexBuffer(vertices, _vertexSize * vertexCount);
            }
            else
            {
                return uploadData(_vertexBuffer, vertices, _vertexSize * vertexCount);
            }

            return true;
        }

        bool MeshBufferMetal::updateIndexFormat()
        {
            switch (_indexSize)
            {
                case 2: _indexFormat = MTLIndexTypeUInt16; break;
                case 4: _indexFormat = MTLIndexTypeUInt32; break;
                default: log("Invalid index size"); return false;
            }

            return true;
        }

        bool MeshBufferMetal::createIndexBuffer(const void* indices, uint32_t size)
        {
            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            _indexBuffer = [rendererMetal->getDevice() newBufferWithLength:size
                                                                   options:MTLResourceCPUCacheModeWriteCombined];

            if (_indexBuffer == Nil)
            {
                log("Failed to create Metal index buffer");
                return false;
            }

            _indexBufferSize = size;

            return uploadData(_indexBuffer, indices, size);
        }

        bool MeshBufferMetal::createVertexBuffer(const void* vertices, uint32_t size)
        {
            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            _vertexBuffer = [rendererMetal->getDevice() newBufferWithLength:size
                                                                    options:MTLResourceCPUCacheModeWriteCombined];

            if (_vertexBuffer == Nil)
            {
                log("Failed to create Metal vertex buffer");
                return false;
            }

            _vertexBufferSize = size;

            return uploadData(_vertexBuffer, vertices, size);
        }

        bool MeshBufferMetal::uploadData(MTLBufferPtr buffer, const void* data, uint32_t size)
        {
            memcpy([buffer contents], data, size);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
