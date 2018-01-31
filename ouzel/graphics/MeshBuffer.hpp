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

        class MeshBuffer
        {
        public:
            MeshBuffer();
            virtual ~MeshBuffer();

            MeshBuffer(const MeshBuffer&) = delete;
            MeshBuffer& operator=(const MeshBuffer&) = delete;

            MeshBuffer(const MeshBuffer&&) = delete;
            MeshBuffer& operator=(const MeshBuffer&&) = delete;

            bool init(uint32_t newIndexSize, const std::shared_ptr<Buffer>& newIndexBuffer,
                      const std::shared_ptr<Buffer>& newVertexBuffer);

            MeshBufferResource* getResource() const { return resource; }

            uint32_t getIndexSize() const;
            bool setIndexSize(uint32_t newIndexSize);

            const std::shared_ptr<Buffer>& getIndexBuffer() const { return indexBuffer; }
            bool setIndexBuffer(const std::shared_ptr<Buffer>& newIndexBuffer);

            const std::shared_ptr<Buffer>& getVertexBuffer() const { return vertexBuffer; }
            bool setVertexBuffer(const std::shared_ptr<Buffer>& newVertexBuffer);

        private:
            MeshBufferResource* resource = nullptr;

            std::shared_ptr<Buffer> indexBuffer;
            std::shared_ptr<Buffer> vertexBuffer;
            uint32_t indexSize = 0;
        };
    } // namespace graphics
} // namespace ouzel
