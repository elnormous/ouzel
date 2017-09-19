// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLBuffer> MTLBufferPtr;
#else
#include <objc/objc.h>
typedef id MTLBufferPtr;
typedef NSUInteger MTLIndexType;
#endif

#include "graphics/MeshBufferResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class BufferResourceMetal;

        class MeshBufferResourceMetal: public MeshBufferResource
        {
        public:
            MeshBufferResourceMetal();
            virtual ~MeshBufferResourceMetal();

            virtual bool init(uint32_t newIndexSize, BufferResource* newIndexBuffer,
                              const std::vector<VertexAttribute>& newVertexAttributes, BufferResource* newVertexBuffer) override;

            virtual bool setIndexSize(uint32_t newIndexSize) override;
            virtual bool setIndexBuffer(BufferResource* newIndexBuffer) override;
            virtual bool setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes) override;
            virtual bool setVertexBuffer(BufferResource* newVertexBuffer) override;

            MTLIndexType getIndexType() const { return indexType; }
            NSUInteger getBytesPerIndex() const { return bytesPerIndex; }

            BufferResourceMetal* getIndexBufferMetal() const { return indexBufferMetal; }
            BufferResourceMetal* getVertexBufferMetal() const { return vertexBufferMetal; }

        protected:
            MTLIndexType indexType;
            NSUInteger bytesPerIndex = 0;

            BufferResourceMetal* indexBufferMetal = nullptr;
            BufferResourceMetal* vertexBufferMetal = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif
