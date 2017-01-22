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

        class MeshBufferResource: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~MeshBufferResource();
            virtual void free() override;

            bool init(const IndexBufferResourcePtr& newIndexBuffer,
                      const VertexBufferResourcePtr& newVertexBuffer);

            void setIndexBuffer(const IndexBufferResourcePtr& newIndexBuffer);
            void setVertexBuffer(const VertexBufferResourcePtr& newVertexBuffer);

            const IndexBufferResourcePtr& getIndexBuffer() const { return indexBuffer; }
            const VertexBufferResourcePtr& getVertexBuffer() const { return vertexBuffer; }

        protected:
            MeshBufferResource();
            virtual void update() override;
            virtual bool upload() override;

            struct Data
            {
                IndexBufferResourcePtr indexBuffer;
                VertexBufferResourcePtr vertexBuffer;
            };

            Data uploadData;
            std::mutex uploadMutex;

        private:
            IndexBufferResourcePtr indexBuffer;
            VertexBufferResourcePtr vertexBuffer;

            Data currentData;
        };
    } // namespace graphics
} // namespace ouzel
