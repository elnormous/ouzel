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
            
            ready = false;
        }

        void MeshBuffer::setIndexBuffer(const IndexBufferPtr& newIndexBuffer)
        {
            indexBuffer = newIndexBuffer;
        }

        void MeshBuffer::setVertexBuffer(const VertexBufferPtr& newVertexBuffer)
        {
            vertexBuffer = newVertexBuffer;
        }
    } // namespace graphics
} // namespace ouzel
