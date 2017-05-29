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
            if (sharedEngine && resource) sharedEngine->getRenderer()->deleteResource(resource);
        }

        bool MeshBuffer::init(uint32_t newIndexSize, const std::shared_ptr<Buffer>& newIndexBuffer,
                              const std::vector<VertexAttribute>& newVertexAttributes, const std::shared_ptr<Buffer>& newVertexBuffer)
        {
            indexBuffer = newIndexBuffer;
            vertexBuffer = newVertexBuffer;

            if (!resource->init(newIndexSize, newIndexBuffer ? newIndexBuffer->getResource() : nullptr,
                                newVertexAttributes, newVertexBuffer ? newVertexBuffer->getResource() : nullptr))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        uint32_t MeshBuffer::getIndexSize() const
        {
            return resource->getIndexSize();
        }

        bool MeshBuffer::setIndexSize(uint32_t newIndexSize)
        {
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

        uint32_t MeshBuffer::getVertexSize() const
        {
            return resource->getVertexSize();
        }

        const std::vector<VertexAttribute>& MeshBuffer::getVertexAttributes() const
        {
            return resource->getVertexAttributes();
        }

        bool MeshBuffer::setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes)
        {
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
