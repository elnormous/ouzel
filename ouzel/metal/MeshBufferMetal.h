// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "MeshBuffer.h"

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLBuffer> MTLBufferPtr;
#else
#include <objc/objc.h>
typedef id MTLBufferPtr;
typedef NSUInteger MTLIndexType;
#endif

namespace ouzel
{
    namespace graphics
    {
        class RendererMetal;

        class MeshBufferMetal: public MeshBuffer
        {
            friend RendererMetal;
        public:
            virtual ~MeshBufferMetal();
            virtual void free() override;

            virtual bool initFromData(const void* newIndices, uint32_t newIndexSize, uint32_t newIndexCount, bool newDynamicIndexBuffer, const void* newVertices, uint32_t newVertexAttributes, uint32_t newVertexCount, bool newDynamicVertexBuffer) override;

            virtual bool setIndexSize(uint32_t indexSize) override;

            virtual bool uploadIndices(const void* indices, uint32_t indexCount) override;
            virtual bool uploadVertices(const void* vertices, uint32_t vertexCount) override;

            MTLBufferPtr getIndexBuffer() const { return indexBuffer; }
            MTLBufferPtr getVertexBuffer() const { return vertexBuffer; }

            MTLIndexType getIndexFormat() const { return indexFormat; }

        protected:
            MeshBufferMetal();

            bool updateIndexFormat();
            bool createIndexBuffer(const void* indices, uint32_t size);
            bool createVertexBuffer(const void* vertices, uint32_t size);
            bool uploadData(MTLBufferPtr buffer, const void* data, uint32_t size);

            MTLBufferPtr indexBuffer = Nil;
            uint32_t indexBufferSize = 0;

            MTLBufferPtr vertexBuffer = Nil;
            uint32_t vertexBufferSize = 0;

            MTLIndexType indexFormat;
        };
    } // namespace graphics
} // namespace ouzel
