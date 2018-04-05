// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include "graphics/Vertex.hpp"

namespace ouzel
{
    namespace graphics
    {
        class MeshBufferResource;
        class Buffer;

        class MeshBuffer final
        {
        public:
            MeshBuffer();
            ~MeshBuffer();

            MeshBuffer(const MeshBuffer&) = delete;
            MeshBuffer& operator=(const MeshBuffer&) = delete;

            MeshBuffer(MeshBuffer&&) = delete;
            MeshBuffer& operator=(MeshBuffer&&) = delete;

            bool init(uint32_t newIndexSize, const std::shared_ptr<Buffer>& newIndexBuffer,
                      const std::shared_ptr<Buffer>& newVertexBuffer);

            inline MeshBufferResource* getResource() const { return resource; }

            inline uint32_t getIndexSize() const { return indexSize; }
            bool setIndexSize(uint32_t newIndexSize);

            inline const std::shared_ptr<Buffer>& getIndexBuffer() const { return indexBuffer; }
            bool setIndexBuffer(const std::shared_ptr<Buffer>& newIndexBuffer);

            inline const std::shared_ptr<Buffer>& getVertexBuffer() const { return vertexBuffer; }
            bool setVertexBuffer(const std::shared_ptr<Buffer>& newVertexBuffer);

        private:
            MeshBufferResource* resource = nullptr;

            std::shared_ptr<Buffer> indexBuffer;
            std::shared_ptr<Buffer> vertexBuffer;
            uint32_t indexSize = 0;
        };
    } // namespace graphics
} // namespace ouzel
