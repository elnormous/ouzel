// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLBuffer> MTLBufferPtr;
#else
#include <objc/objc.h>
typedef id MTLBufferPtr;
#endif

#include "graphics/BufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        class MeshBufferMetal;

        class BufferMetal: public BufferResource
        {
            friend MeshBufferMetal;
        public:
            BufferMetal();
            virtual ~BufferMetal();

            MTLBufferPtr getBuffer() const { return buffer; }

        protected:
            virtual bool upload() override;

            MTLBufferPtr buffer = Nil;
            uint32_t bufferSize = 0;
        };
    } // namespace graphics
} // namespace ouzel
