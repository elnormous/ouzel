// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <mutex>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
#include "graphics/Vertex.h"
#include "utils/Types.h"

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

            bool init(uint32_t newIndexSize, BufferResource* newIndexBuffer,
                      const std::vector<VertexAttribute>& newVertexAttributes, BufferResource* newVertexBuffer);

            uint32_t getIndexSize() const { return data.indexSize; }
            bool setIndexSize(uint32_t newIndexSize);

            BufferResource* getIndexBuffer() const { return data.indexBuffer; }
            bool setIndexBuffer(BufferResource* newIndexBuffer);

            uint32_t getVertexSize() const { return data.vertexSize; }
            bool setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes);

            BufferResource* getVertexBuffer() const { return data.vertexBuffer; }
            bool setVertexBuffer(BufferResource* newVertexBuffer);

        protected:
            MeshBufferResource();
            virtual bool upload() override;

            enum Dirty
            {
                INDEX_ATTRIBUTES = 0x01,
                INDEX_BUFFER = 0x02,
                VERTEX_ATTRIBUTES = 0x04,
                VERTEX_BUFFER = 0x08
            };

            struct Data
            {
                uint32_t dirty = 0;
                uint32_t indexSize = 0;
                BufferResource* indexBuffer = nullptr;
                std::vector<VertexAttribute> vertexAttributes;
                uint32_t vertexSize = 0;
                BufferResource* vertexBuffer = nullptr;
            };

            Data data;

        private:
            Data pendingData;
            std::mutex uploadMutex;
        };
    } // namespace graphics
} // namespace ouzel
