// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferResource.h"

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

        bool MeshBufferResource::init(const IndexBufferResourcePtr& newIndexBuffer,
                                      const VertexBufferResourcePtr& newVertexBuffer)
        {
            indexBuffer = newIndexBuffer;
            vertexBuffer = newVertexBuffer;

            update();

            return true;
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

            currentData.indexBuffer = indexBuffer;
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
