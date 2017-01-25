// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "utils/Noncopyable.h"

namespace ouzel
{
    namespace graphics
    {
        class MeshBufferResource;
        class Buffer;

        class MeshBuffer: public Noncopyable
        {
        public:
            MeshBuffer();
            virtual ~MeshBuffer();

            bool init(uint32_t newIndexSize, const std::shared_ptr<Buffer>& newIndexBuffer,
                      uint32_t newVertexAttributes, const std::shared_ptr<Buffer>& newVertexBuffer);

            MeshBufferResource* getResource() const { return resource; }

            uint32_t getIndexSize() const { return indexSize; }
            bool setIndexSize(uint32_t newIndexSize);

            const std::shared_ptr<Buffer>& getIndexBuffer() const { return indexBuffer; }
            bool setIndexBuffer(const std::shared_ptr<Buffer>& newIndexBuffer);

            uint32_t getVertexAttributes() const { return vertexAttributes; }
            bool setVertexAttributes(uint32_t newVertexAttributes);

            const std::shared_ptr<Buffer>& getVertexBuffer() const { return vertexBuffer; }
            bool setVertexBuffer(const std::shared_ptr<Buffer>& newVertexBuffer);

        private:
            MeshBufferResource* resource = nullptr;
            uint32_t indexSize = 0;
            std::shared_ptr<Buffer> indexBuffer;
            uint32_t vertexAttributes = 0;
            std::shared_ptr<Buffer> vertexBuffer;
        };
    } // namespace graphics
} // namespace ouzel
