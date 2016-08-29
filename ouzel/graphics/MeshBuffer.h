// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <atomic>
#include <mutex>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
#include "graphics/Vertex.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class MeshBuffer: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~MeshBuffer();
            virtual void free() override;

            virtual bool init(bool newDynamicIndexBuffer = true, bool newDynamicVertexBuffer = true);
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

            virtual bool setIndices(const void* newIndices, uint32_t newIndexCount);
            virtual bool setVertices(const void* newVertices, uint32_t newVertexCount);

            bool isReady() const { return ready; }

        protected:
            MeshBuffer();
            void updateVertexSize();

            virtual bool update() override;

            uint32_t indexCount = 0;
            uint32_t indexSize = 0;

            uint32_t vertexCount = 0;
            uint32_t vertexSize = 0;

            uint32_t vertexAttributes;

            std::vector<uint8_t> indexData;
            std::vector<uint8_t> vertexData;

            struct Data
            {
                uint32_t indexSize = 0;
                uint32_t vertexSize = 0;
                uint32_t vertexAttributes = 0;
                bool dynamicIndexBuffer = true;
                bool dynamicVertexBuffer = true;
                std::vector<uint8_t> indexData;
                std::vector<uint8_t> vertexData;
            };

            Data uploadData;

            bool dynamicIndexBuffer = true;
            bool dynamicVertexBuffer = true;
            bool ready = false;

            bool indexBufferDirty = false;
            bool vertexBufferDirty = false;
            bool indexSizeDirty = false;
            bool vertexAttributesDirty = false;
        };
    } // namespace graphics
} // namespace ouzel
