// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class VertexBuffer: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~VertexBuffer();
            virtual void free() override;

            virtual bool init(bool newDynamic = true);
            virtual bool initFromBuffer(const void* newVertices, uint32_t newVertexAttributes,
                                        uint32_t newVertexCount, bool newDynamic);

            uint32_t getVertexCount() const { return vertexCount; }
            uint32_t getVertexSize() const { return vertexSize; }

            virtual bool setVertexAttributes(uint32_t newVertexAttributes);
            uint32_t getVertexAttributes() const { return vertexAttributes; }

            virtual bool setData(const void* newVertices, uint32_t newVertexCount);

        protected:
            VertexBuffer();
            void updateVertexSize();
            virtual void update() override;

            enum Dirty
            {
                VERTEX_BUFFER_DIRTY = 0x01,
                VERTEX_ATTRIBUTES_DIRTY = 0x02,
            };

            struct Data
            {
                uint32_t vertexSize = 0;
                uint32_t vertexAttributes = 0;
                std::vector<uint8_t> data;
                bool dynamic = true;
                uint8_t dirty = false;
            };

            Data uploadData;

        private:
            uint32_t vertexCount = 0;
            uint32_t vertexSize = 0;

            uint32_t vertexAttributes = 0;

            std::vector<uint8_t> data;

            bool dynamic = true;

            uint8_t dirty = 0;
        };
    } // namespace graphics
} // namespace ouzel
