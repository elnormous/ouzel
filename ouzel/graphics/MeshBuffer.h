// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <atomic>
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

        class MeshBuffer: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~MeshBuffer();
            virtual void free() override;
            virtual bool upload() override;

            bool init(const IndexBufferPtr& newIndexBuffer,
                      const VertexBufferPtr& newVertexBuffer);

            void setIndexBuffer(const IndexBufferPtr& newIndexBuffer);
            void setVertexBuffer(const VertexBufferPtr& newVertexBuffer);

            const IndexBufferPtr& getIndexBuffer() const { return indexBuffer; }
            const VertexBufferPtr& getVertexBuffer() const { return vertexBuffer; }

            bool isReady() const { return ready; }

        protected:
            MeshBuffer();

            IndexBufferPtr indexBuffer;
            VertexBufferPtr vertexBuffer;

            bool ready = false;
            bool dirty = false;
        };
    } // namespace graphics
} // namespace ouzel
