// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLBuffer> MTLBufferPtr;
#else
#include <objc/objc.h>
typedef id MTLBufferPtr;
typedef NSUInteger MTLIndexType;
#endif

#include "graphics/IndexBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererMetal;
        
        class IndexBufferMetal: public IndexBuffer
        {
            friend RendererMetal;
        public:
            virtual ~IndexBufferMetal();
            virtual void free() override;

            MTLBufferPtr getBuffer() const { return buffer; }
            MTLIndexType getType() const { return type; }
            NSUInteger getBytesPerIndex() const { return bytesPerIndex; }

        protected:
            IndexBufferMetal();
            virtual bool upload() override;

            MTLBufferPtr buffer = Nil;
            uint32_t bufferSize = 0;

            MTLIndexType type;
            NSUInteger bytesPerIndex = 0;
        };
    } // namespace graphics
} // namespace ouzel
