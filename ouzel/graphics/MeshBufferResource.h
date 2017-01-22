// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLBuffer> MTLBufferPtr;
#else
#include <objc/objc.h>
#include <objc/NSObjCRuntime.h>
typedef id MTLBufferPtr;
typedef NSUInteger MTLIndexType;
#endif
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

            bool init(uint32_t newIndexSize, const IndexBufferResourcePtr& newIndexBuffer,
                      uint32_t newVertexAttributes, const VertexBufferResourcePtr& newVertexBuffer);

            bool setIndexSize(uint32_t newIndexSize);
            uint32_t getIndexSize() const { return indexSize; }

            bool setVertexAttributes(uint32_t newVertexAttributes);
            uint32_t getVertexAttributes() const { return vertexAttributes; }
            uint32_t getVertexSize() const { return vertexSize; }

            void setIndexBuffer(const IndexBufferResourcePtr& newIndexBuffer);
            void setVertexBuffer(const VertexBufferResourcePtr& newVertexBuffer);

            const IndexBufferResourcePtr& getIndexBuffer() const { return indexBuffer; }
            const VertexBufferResourcePtr& getVertexBuffer() const { return vertexBuffer; }

        protected:
            MeshBufferResource();
            void updateVertexSize();
            virtual void update() override;
            virtual bool upload() override;

            struct Data
            {
                uint32_t indexSize = 0;
                IndexBufferResourcePtr indexBuffer;
                uint32_t vertexAttributes = 0;
                uint32_t vertexSize = 0;
                VertexBufferResourcePtr vertexBuffer;
            };

            Data uploadData;
            std::mutex uploadMutex;

        private:
            uint32_t indexSize = 0;
            IndexBufferResourcePtr indexBuffer;

            uint32_t vertexAttributes = 0;
            uint32_t vertexSize = 0;
            VertexBufferResourcePtr vertexBuffer;

            Data currentData;
        };
    } // namespace graphics
} // namespace ouzel
