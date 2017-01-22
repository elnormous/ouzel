// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferResource.h"
#include "Vertex.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferResource::MeshBufferResource()
        {
        }

        MeshBufferResource::~MeshBufferResource()
        {
        }

        void MeshBufferResource::free()
        {
            indexBuffer.reset();
            vertexBuffer.reset();
        }

        bool MeshBufferResource::init(uint32_t newIndexSize, const IndexBufferResourcePtr& newIndexBuffer,
                                      uint32_t newVertexAttributes, const VertexBufferResourcePtr& newVertexBuffer)
        {
            indexSize = newIndexSize;
            indexBuffer = newIndexBuffer;
            vertexAttributes = newVertexAttributes;
            vertexBuffer = newVertexBuffer;
            updateVertexSize();

            update();

            return true;
        }

        bool MeshBufferResource::setIndexSize(uint32_t newIndexSize)
        {
            indexSize = newIndexSize;

            update();

            return true;
        }

        bool MeshBufferResource::setVertexAttributes(uint32_t newVertexAttributes)
        {
            vertexAttributes = newVertexAttributes;
            updateVertexSize();

            update();

            return true;
        }

        void MeshBufferResource::updateVertexSize()
        {
            vertexSize = 0;

            if (vertexAttributes & VERTEX_POSITION)
            {
                vertexSize += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_COLOR)
            {
                vertexSize += 4 * sizeof(uint8_t);
            }

            if (vertexAttributes & VERTEX_NORMAL)
            {
                vertexSize += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD0)
            {
                vertexSize += 2 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD1)
            {
                vertexSize += 2 * sizeof(float);
            }
        }

        void MeshBufferResource::setIndexBuffer(const IndexBufferResourcePtr& newIndexBuffer)
        {
            indexBuffer = newIndexBuffer;

            update();
        }

        void MeshBufferResource::setVertexBuffer(const VertexBufferResourcePtr& newVertexBuffer)
        {
            vertexBuffer = newVertexBuffer;

            update();
        }

        void MeshBufferResource::update()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            currentData.indexSize = indexSize;
            currentData.indexBuffer = indexBuffer;
            currentData.vertexAttributes = vertexAttributes;
            currentData.vertexSize = vertexSize;
            currentData.vertexBuffer = vertexBuffer;

            dirty = true;
        }

        bool MeshBufferResource::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            dirty = false;
            uploadData = std::move(currentData);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
