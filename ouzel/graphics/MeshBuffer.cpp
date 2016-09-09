// Copyright (C) 2016 Elviss Strazdins
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

            dirty = true;
            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());
            
            return true;
        }

        void MeshBuffer::setIndexBuffer(const IndexBufferPtr& newIndexBuffer)
        {
            indexBuffer = newIndexBuffer;

            dirty = true;
            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());
        }

        void MeshBuffer::setVertexBuffer(const VertexBufferPtr& newVertexBuffer)
        {
            vertexBuffer = newVertexBuffer;

            dirty = true;
            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());
        }

        void MeshBuffer::update()
        {
            uploadData.indexBuffer = indexBuffer;
            uploadData.vertexBuffer = vertexBuffer;
            uploadData.dirty = dirty;

            dirty = false;
        }
    } // namespace graphics
} // namespace ouzel
