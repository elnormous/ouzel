// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/ResourceInterface.hpp"
#include "graphics/Vertex.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class BufferInterface;

        class MeshBufferInterface: public ResourceInterface
        {
            friend Renderer;
        public:
            virtual ~MeshBufferInterface();

            virtual bool init(uint32_t newIndexSize, BufferInterface* newIndexBuffer,
                              const std::vector<VertexAttribute>& newVertexAttributes, BufferInterface* newVertexBuffer);

            uint32_t getIndexSize() const { return indexSize; }
            virtual bool setIndexSize(uint32_t newIndexSize);

            BufferInterface* getIndexBuffer() const { return indexBuffer; }
            virtual bool setIndexBuffer(BufferInterface* newIndexBuffer);

            uint32_t getVertexSize() const { return vertexSize; }
            const std::vector<VertexAttribute>& getVertexAttributes() const { return vertexAttributes; }
            virtual bool setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes);

            BufferInterface* getVertexBuffer() const { return vertexBuffer; }
            virtual bool setVertexBuffer(BufferInterface* newVertexBuffer);

        protected:
            MeshBufferInterface();

            uint32_t indexSize = 0;
            BufferInterface* indexBuffer = nullptr;
            std::vector<VertexAttribute> vertexAttributes;
            uint32_t vertexSize = 0;
            BufferInterface* vertexBuffer = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
