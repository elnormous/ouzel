// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/RenderResource.hpp"
#include "graphics/Vertex.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class BufferResource;

        class MeshBufferResource: public RenderResource
        {
            friend Renderer;
        public:
            virtual ~MeshBufferResource();

            virtual bool init(uint32_t newIndexSize, BufferResource* newIndexBuffer,
                              BufferResource* newVertexBuffer);

            uint32_t getIndexSize() const { return indexSize; }
            virtual bool setIndexSize(uint32_t newIndexSize);

            BufferResource* getIndexBuffer() const { return indexBuffer; }
            virtual bool setIndexBuffer(BufferResource* newIndexBuffer);

            BufferResource* getVertexBuffer() const { return vertexBuffer; }
            virtual bool setVertexBuffer(BufferResource* newVertexBuffer);

        protected:
            MeshBufferResource();

            uint32_t indexSize = 0;
            BufferResource* indexBuffer = nullptr;
            BufferResource* vertexBuffer = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
