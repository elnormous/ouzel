// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "MeshBuffer.h"

#ifdef __OBJC__
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

            virtual bool initFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer) override;

            virtual bool setIndexSize(uint32_t indexSize) override;

            virtual bool uploadIndices(const void* indices, uint32_t indexCount) override;
            virtual bool uploadVertices(const void* vertices, uint32_t vertexCount) override;

            MTLBufferPtr getIndexBuffer() const { return _indexBuffer; }
            MTLBufferPtr getVertexBuffer() const { return _vertexBuffer; }

            MTLIndexType getIndexFormat() const { return _indexFormat; }

        protected:
            MeshBufferMetal();

            void destroy();
            bool updateIndexFormat();
            bool createIndexBuffer(const void* indices, uint32_t size);
            bool createVertexBuffer(const void* vertices, uint32_t size);
            bool uploadData(MTLBufferPtr buffer, const void* data, uint32_t size);

            MTLBufferPtr _indexBuffer = Nil;
            uint32_t _indexBufferSize = 0;

            MTLBufferPtr _vertexBuffer = Nil;
            uint32_t _vertexBufferSize = 0;

            MTLIndexType _indexFormat;
        };
    } // namespace graphics
} // namespace ouzel
