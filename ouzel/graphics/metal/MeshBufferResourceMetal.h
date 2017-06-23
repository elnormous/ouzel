// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLBuffer> MTLBufferPtr;
#else
#include <objc/objc.h>
typedef id MTLBufferPtr;
typedef NSUInteger MTLIndexType;
#endif
#include "graphics/MeshBufferResource.h"

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

            MTLIndexType getIndexType() const { return indexType; }
            NSUInteger getBytesPerIndex() const { return bytesPerIndex; }

            BufferResourceMetal* getIndexBufferMetal() const { return indexBufferMetal; }
            BufferResourceMetal* getVertexBufferMetal() const { return vertexBufferMetal; }

        protected:
            virtual bool upload() override;

            MTLIndexType indexType;
            NSUInteger bytesPerIndex = 0;

            BufferResourceMetal* indexBufferMetal = nullptr;
            BufferResourceMetal* vertexBufferMetal = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif
