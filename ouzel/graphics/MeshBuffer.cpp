// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBuffer.h"
#include "MeshBufferResource.h"
#include "Buffer.h"
#include "Renderer.h"
#include "core/Engine.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBuffer::MeshBuffer()
        {
            resource = sharedEngine->getRenderer()->createMeshBuffer();
        }

        MeshBuffer::~MeshBuffer()
        {
            if (resource) sharedEngine->getRenderer()->deleteResource(resource);
        }

        bool MeshBuffer::init(uint32_t newIndexSize, const std::shared_ptr<Buffer>& newIndexBuffer,
                              const std::vector<VertexAttribute>& newVertexAttributes, const std::shared_ptr<Buffer>& newVertexBuffer)
        {
            if (newIndexBuffer && newIndexBuffer->getUsage() != Buffer::Usage::INDEX)
            {
                return false;
            }

            if (newVertexBuffer && newVertexBuffer->getUsage() != Buffer::Usage::VERTEX)
            {
                return false;
            }

            indexSize = newIndexSize;
            indexBuffer = newIndexBuffer;

            vertexAttributes = newVertexAttributes;
            vertexSize = 0;

            for (const VertexAttribute& vertexAttribute : vertexAttributes)
            {
                vertexSize += getDataTypeSize(vertexAttribute.dataType);
            }

            vertexBuffer = newVertexBuffer;

            if (!resource->init(newIndexSize, newIndexBuffer ? newIndexBuffer->getResource() : nullptr,
                                newVertexAttributes, newVertexBuffer ? newVertexBuffer->getResource() : nullptr))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool MeshBuffer::setIndexSize(uint32_t newIndexSize)
        {
            indexSize = newIndexSize;

            if (!resource->setIndexSize(newIndexSize))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool MeshBuffer::setIndexBuffer(const std::shared_ptr<Buffer>& newIndexBuffer)
        {
            indexBuffer = newIndexBuffer;

            if (!resource->setIndexBuffer(newIndexBuffer ? newIndexBuffer->getResource() : nullptr))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool MeshBuffer::setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes)
        {
            vertexAttributes = newVertexAttributes;
            vertexSize = 0;

            for (const VertexAttribute& vertexAttribute : vertexAttributes)
            {
                vertexSize += getDataTypeSize(vertexAttribute.dataType);
            }

            if (!resource->setVertexAttributes(newVertexAttributes))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool MeshBuffer::setVertexBuffer(const std::shared_ptr<Buffer>& newVertexBuffer)
        {
            vertexBuffer = newVertexBuffer;

            if (!resource->setVertexBuffer(newVertexBuffer ? newVertexBuffer->getResource() : nullptr))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
