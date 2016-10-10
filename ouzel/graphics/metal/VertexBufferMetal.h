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

#include "graphics/VertexBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class VertexBufferMetal: public VertexBuffer
        {
        public:
            VertexBufferMetal();
            virtual ~VertexBufferMetal();
            virtual void free() override;

            MTLBufferPtr getBuffer() const { return buffer; }

        protected:
            virtual bool upload() override;

            MTLBufferPtr buffer = Nil;
            uint32_t bufferSize = 0;
        };
    } // namespace graphics
} // namespace ouzel
