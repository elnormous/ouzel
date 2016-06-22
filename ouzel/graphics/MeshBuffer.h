// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "utils/Noncopyable.h"
#include "graphics/Vertex.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class MeshBuffer: public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~MeshBuffer();
            virtual void free();

            virtual bool init();
            virtual bool initFromBuffer(const void* newIndices, uint32_t newIndexSize,
                                        uint32_t newIndexCount, bool newDynamicIndexBuffer,
                                        const void* newVertices, uint32_t newVertexAttributes,
                                        uint32_t newVertexCount, bool newDynamicVertexBuffer);

            uint32_t getIndexCount() const { return indexCount; }
            virtual bool setIndexSize(uint32_t newIndexSize);
            uint32_t getIndexSize() const { return indexSize; }

            uint32_t getVertexCount() const { return vertexCount; }
            uint32_t getVertexSize() const { return vertexSize; }

            virtual bool setVertexAttributes(uint32_t newVertexAttributes);
            uint32_t getVertexAttributes() const { return vertexAttributes; }

            virtual bool uploadIndices(const void* newIndices, uint32_t newIndexCount);
            virtual bool uploadVertices(const void* newVertices, uint32_t newVertexCount);

            bool isReady() const { return ready; }

        protected:
            MeshBuffer();
            void updateVertexSize();

            uint32_t indexCount = 0;
            uint32_t indexSize = 0;
            bool dynamicIndexBuffer = true;

            uint32_t vertexCount = 0;
            uint32_t vertexSize = 0;
            bool dynamicVertexBuffer = true;

            uint32_t vertexAttributes;

            bool ready = false;
        };
    } // namespace graphics
} // namespace ouzel
