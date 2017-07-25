// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <mutex>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
#include "graphics/Vertex.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class BufferResource;

        class MeshBufferResource: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~MeshBufferResource();

            virtual bool init(uint32_t newIndexSize, BufferResource* newIndexBuffer,
                              const std::vector<VertexAttribute>& newVertexAttributes, BufferResource* newVertexBuffer);

            uint32_t getIndexSize() const { return indexSize; }
            virtual bool setIndexSize(uint32_t newIndexSize);

            BufferResource* getIndexBuffer() const { return indexBuffer; }
            virtual bool setIndexBuffer(BufferResource* newIndexBuffer);

            uint32_t getVertexSize() const { return vertexSize; }
            const std::vector<VertexAttribute>& getVertexAttributes() const { return vertexAttributes; }
            virtual bool setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes);

            BufferResource* getVertexBuffer() const { return vertexBuffer; }
            virtual bool setVertexBuffer(BufferResource* newVertexBuffer);

        protected:
            MeshBufferResource();

            uint32_t indexSize = 0;
            BufferResource* indexBuffer = nullptr;
            std::vector<VertexAttribute> vertexAttributes;
            uint32_t vertexSize = 0;
            BufferResource* vertexBuffer = nullptr;

            std::mutex uploadMutex;
        };
    } // namespace graphics
} // namespace ouzel
