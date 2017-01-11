// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <mutex>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
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

            bool init(const IndexBufferPtr& newIndexBuffer,
                      const VertexBufferPtr& newVertexBuffer);

            void setIndexBuffer(const IndexBufferPtr& newIndexBuffer);
            void setVertexBuffer(const VertexBufferPtr& newVertexBuffer);

            const IndexBufferPtr& getIndexBuffer() const { return indexBuffer; }
            const VertexBufferPtr& getVertexBuffer() const { return vertexBuffer; }

        protected:
            MeshBuffer();
            virtual void update() override;
            virtual bool upload() override;

            struct Data
            {
                IndexBufferPtr indexBuffer;
                VertexBufferPtr vertexBuffer;
            };

            Data uploadData;
            std::mutex uploadMutex;

        private:
            IndexBufferPtr indexBuffer;
            VertexBufferPtr vertexBuffer;

            Data currentData;
        };
    } // namespace graphics
} // namespace ouzel
