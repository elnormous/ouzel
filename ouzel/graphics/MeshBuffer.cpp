// Copyright (C) 2018 Elviss Strazdins
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
            resource = engine->getRenderer()->getDevice()->createMeshBuffer();
        }

        MeshBuffer::~MeshBuffer()
        {
            if (engine && resource) engine->getRenderer()->getDevice()->deleteResource(resource);
        }

        bool MeshBuffer::init(uint32_t newIndexSize, const std::shared_ptr<Buffer>& newIndexBuffer,
                              const std::shared_ptr<Buffer>& newVertexBuffer)
        {
            indexBuffer = newIndexBuffer;
            vertexBuffer = newVertexBuffer;
            indexSize = newIndexSize;

            BufferResource* indexBufferResource = newIndexBuffer ? newIndexBuffer->getResource() : nullptr;
            BufferResource* vertexBufferResource = newVertexBuffer ? newVertexBuffer->getResource() : nullptr;

            engine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferResource::init,
                                                                   resource,
                                                                   newIndexSize,
                                                                   indexBufferResource,
                                                                   vertexBufferResource));

            return true;
        }

        bool MeshBuffer::setIndexSize(uint32_t newIndexSize)
        {
            indexSize = newIndexSize;

            engine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferResource::setIndexSize,
                                                                   resource,
                                                                   newIndexSize));

            return true;
        }

        bool MeshBuffer::setIndexBuffer(const std::shared_ptr<Buffer>& newIndexBuffer)
        {
            indexBuffer = newIndexBuffer;

            BufferResource* indexBufferResource = indexBuffer ? indexBuffer->getResource() : nullptr;

            engine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferResource::setIndexBuffer,
                                                                   resource,
                                                                   indexBufferResource));

            return true;
        }

        bool MeshBuffer::setVertexBuffer(const std::shared_ptr<Buffer>& newVertexBuffer)
        {
            vertexBuffer = newVertexBuffer;

            BufferResource* vertexBufferResource = vertexBuffer ? vertexBuffer->getResource() : nullptr;

            engine->getRenderer()->executeOnRenderThread(std::bind(&MeshBufferResource::setVertexBuffer,
                                                                   resource,
                                                                   vertexBufferResource));

            return true;
        }
    } // namespace graphics
} // namespace ouzel
