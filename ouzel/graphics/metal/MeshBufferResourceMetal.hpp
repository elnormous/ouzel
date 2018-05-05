// Copyright (C) 2018 Elviss Strazdins
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
                              BufferResource* newVertexBuffer) override;

            virtual bool setIndexSize(uint32_t newIndexSize) override;
            virtual bool setIndexBuffer(BufferResource* newIndexBuffer) override;
            virtual bool setVertexBuffer(BufferResource* newVertexBuffer) override;

            inline MTLIndexType getIndexType() const { return indexType; }
            inline NSUInteger getBytesPerIndex() const { return bytesPerIndex; }

            inline BufferResourceMetal* getIndexBufferMetal() const { return indexBufferMetal; }
            inline BufferResourceMetal* getVertexBufferMetal() const { return vertexBufferMetal; }

        private:
            MTLIndexType indexType;
            NSUInteger bytesPerIndex = 0;

            BufferResourceMetal* indexBufferMetal = nullptr;
            BufferResourceMetal* vertexBufferMetal = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif
