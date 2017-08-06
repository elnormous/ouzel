// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBuffer.hpp"
#include "MeshBufferInterface.hpp"
#include "Buffer.hpp"
#include "Renderer.hpp"
#include "core/Engine.hpp"

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

            BufferInterface* indexBufferInterface = newIndexBuffer ? newIndexBuffer->getResource() : nullptr;
            BufferInterface* vertexBufferInterface = newVertexBuffer ? newVertexBuffer->getResource() : nullptr;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferInterface::init,
                                                                         resource,
                                                                         newIndexSize,
                                                                         indexBufferInterface,
                                                                         newVertexAttributes,
                                                                         vertexBufferInterface));

            return true;
        }

        uint32_t MeshBuffer::getIndexSize() const
        {
            return indexSize;
        }

        bool MeshBuffer::setIndexSize(uint32_t newIndexSize)
        {
            indexSize = newIndexSize;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferInterface::setIndexSize,
                                                                         resource,
                                                                         newIndexSize));

            return true;
        }

        bool MeshBuffer::setIndexBuffer(const std::shared_ptr<Buffer>& newIndexBuffer)
        {
            indexBuffer = newIndexBuffer;

            BufferInterface* indexBufferResource = indexBuffer ? indexBuffer->getResource() : nullptr;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferInterface::setIndexBuffer,
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

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferInterface::setVertexAttributes,
                                                                         resource,
                                                                         newVertexAttributes));

            return true;
        }

        bool MeshBuffer::setVertexBuffer(const std::shared_ptr<Buffer>& newVertexBuffer)
        {
            vertexBuffer = newVertexBuffer;

            BufferInterface* vertexBufferInterface = vertexBuffer ? vertexBuffer->getResource() : nullptr;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferInterface::setVertexBuffer,
                                                                         resource,
                                                                         vertexBufferInterface));

            return true;
        }
    } // namespace graphics
} // namespace ouzel
