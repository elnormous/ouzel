// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBuffer.h"
#include "core/Engine.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBuffer::MeshBuffer()
        {
        }

        MeshBuffer::~MeshBuffer()
        {
        }

        void MeshBuffer::free()
        {
            indexBuffer.reset();
            vertexBuffer.reset();
        }

        bool MeshBuffer::init(const IndexBufferPtr& newIndexBuffer,
                              const VertexBufferPtr& newVertexBuffer)
        {
            indexBuffer = newIndexBuffer;
            vertexBuffer = newVertexBuffer;

            update();

            return true;
        }

        void MeshBuffer::setIndexBuffer(const IndexBufferPtr& newIndexBuffer)
        {
            indexBuffer = newIndexBuffer;

            update();
        }

        void MeshBuffer::setVertexBuffer(const VertexBufferPtr& newVertexBuffer)
        {
            vertexBuffer = newVertexBuffer;

            update();
        }

        void MeshBuffer::update()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);
            
            currentData.indexBuffer = indexBuffer;
            currentData.vertexBuffer = vertexBuffer;

            dirty = true;
        }

        bool MeshBuffer::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            dirty = false;
            uploadData = std::move(currentData);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
