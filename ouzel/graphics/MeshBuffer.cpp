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
            indexSize = newIndexSize;
            vertexAttributes = newVertexAttributes;

            MeshBufferResource* meshBufferResource = resource;
            BufferResource* indexBufferResource = newIndexBuffer ? newIndexBuffer->getResource() : nullptr;
            BufferResource* vertexBufferResource = newVertexBuffer ? newVertexBuffer->getResource() : nullptr;

            sharedEngine->getRenderer()->executeOnRenderThread([meshBufferResource,
                                                                newIndexSize,
                                                                indexBufferResource,
                                                                newVertexAttributes,
                                                                vertexBufferResource]() {
                meshBufferResource->init(newIndexSize, indexBufferResource,
                                         newVertexAttributes, vertexBufferResource);
            });

            return true;
        }

        uint32_t MeshBuffer::getIndexSize() const
        {
            return indexSize;
        }

        bool MeshBuffer::setIndexSize(uint32_t newIndexSize)
        {
            indexSize = newIndexSize;

            MeshBufferResource* meshBufferResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([meshBufferResource,
                                                                newIndexSize]() {
                meshBufferResource->setIndexSize(newIndexSize);
            });

            return true;
        }

        bool MeshBuffer::setIndexBuffer(const std::shared_ptr<Buffer>& newIndexBuffer)
        {
            indexBuffer = newIndexBuffer;

            MeshBufferResource* meshBufferResource = resource;
            BufferResource* indexBufferResource = indexBuffer ? indexBuffer->getResource() : nullptr;

            sharedEngine->getRenderer()->executeOnRenderThread([meshBufferResource,
                                                                indexBufferResource]() {
                meshBufferResource->setIndexBuffer(indexBufferResource);
            });

            return true;
        }

        const std::vector<VertexAttribute>& MeshBuffer::getVertexAttributes() const
        {
            return vertexAttributes;
        }

        bool MeshBuffer::setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes)
        {
            vertexAttributes = newVertexAttributes;

            MeshBufferResource* meshBufferResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([meshBufferResource,
                                                                newVertexAttributes]() {
                meshBufferResource->setVertexAttributes(newVertexAttributes);
            });

            return true;
        }

        bool MeshBuffer::setVertexBuffer(const std::shared_ptr<Buffer>& newVertexBuffer)
        {
            vertexBuffer = newVertexBuffer;

            MeshBufferResource* meshBufferResource = resource;
            BufferResource* vertexBufferResource = vertexBuffer ? vertexBuffer->getResource() : nullptr;

            sharedEngine->getRenderer()->executeOnRenderThread([meshBufferResource,
                                                                vertexBufferResource]() {
                meshBufferResource->setVertexBuffer(vertexBufferResource);
            });

            return true;
        }
    } // namespace graphics
} // namespace ouzel
