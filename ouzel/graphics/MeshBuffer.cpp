// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBuffer.hpp"
#include "MeshBufferResource.hpp"
#include "Buffer.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace graphics
    {
        MeshBuffer::MeshBuffer()
        {
            resource = sharedEngine->getRenderer()->getDevice()->createMeshBuffer();
        }

        MeshBuffer::~MeshBuffer()
        {
            if (sharedEngine && resource) sharedEngine->getRenderer()->getDevice()->deleteResource(resource);
        }

        bool MeshBuffer::init(uint32_t newIndexSize, const std::shared_ptr<Buffer>& newIndexBuffer,
                              const std::vector<VertexAttribute>& newVertexAttributes, const std::shared_ptr<Buffer>& newVertexBuffer)
        {
            indexBuffer = newIndexBuffer;
            vertexBuffer = newVertexBuffer;
            indexSize = newIndexSize;
            vertexAttributes = newVertexAttributes;

            BufferResource* indexBufferResource = newIndexBuffer ? newIndexBuffer->getResource() : nullptr;
            BufferResource* vertexBufferResource = newVertexBuffer ? newVertexBuffer->getResource() : nullptr;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferResource::init,
                                                                         resource,
                                                                         newIndexSize,
                                                                         indexBufferResource,
                                                                         newVertexAttributes,
                                                                         vertexBufferResource));

            return true;
        }

        uint32_t MeshBuffer::getIndexSize() const
        {
            return indexSize;
        }

        bool MeshBuffer::setIndexSize(uint32_t newIndexSize)
        {
            indexSize = newIndexSize;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferResource::setIndexSize,
                                                                         resource,
                                                                         newIndexSize));

            return true;
        }

        bool MeshBuffer::setIndexBuffer(const std::shared_ptr<Buffer>& newIndexBuffer)
        {
            indexBuffer = newIndexBuffer;

            BufferResource* indexBufferResource = indexBuffer ? indexBuffer->getResource() : nullptr;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferResource::setIndexBuffer,
                                                                         resource,
                                                                         indexBufferResource));

            return true;
        }

        const std::vector<VertexAttribute>& MeshBuffer::getVertexAttributes() const
        {
            return vertexAttributes;
        }

        bool MeshBuffer::setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes)
        {
            vertexAttributes = newVertexAttributes;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferResource::setVertexAttributes,
                                                                         resource,
                                                                         newVertexAttributes));

            return true;
        }

        bool MeshBuffer::setVertexBuffer(const std::shared_ptr<Buffer>& newVertexBuffer)
        {
            vertexBuffer = newVertexBuffer;

            BufferResource* vertexBufferResource = vertexBuffer ? vertexBuffer->getResource() : nullptr;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferResource::setVertexBuffer,
                                                                         resource,
                                                                         vertexBufferResource));

            return true;
        }
    } // namespace graphics
} // namespace ouzel
