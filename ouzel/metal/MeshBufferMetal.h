// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include "core/CompileConfig.h"
#include "graphics/MeshBuffer.h"

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

            MTLBufferPtr getIndexBuffer() const { return indexBuffer; }
            MTLBufferPtr getVertexBuffer() const { return vertexBuffer; }

            MTLIndexType getIndexFormat() const { return indexFormat; }
            NSUInteger getBytesPerIndex() const { return bytesPerIndex; }

        protected:
            MeshBufferMetal();
            virtual bool update() override;
            virtual bool upload() override;

            bool uploadBuffer(MTLBufferPtr buffer, const std::vector<uint8_t>& data);

            MTLBufferPtr indexBuffer = Nil;
            uint32_t indexBufferSize = 0;

            MTLBufferPtr vertexBuffer = Nil;
            uint32_t vertexBufferSize = 0;

            MTLIndexType indexFormat;
            NSUInteger bytesPerIndex = 0;
        };
    } // namespace graphics
} // namespace ouzel
