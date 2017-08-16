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
#endif

#include "graphics/BufferResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RendererMetal;
        class MeshBufferResourceMetal;

        class BufferResourceMetal: public BufferResource
        {
            friend MeshBufferResourceMetal;
        public:
            BufferResourceMetal(RendererMetal* aRendererMetal);
            virtual ~BufferResourceMetal();

            virtual bool init(Buffer::Usage newUsage, uint32_t newFlags = 0, uint32_t newSize = 0) override;
            virtual bool init(Buffer::Usage newUsage, const std::vector<uint8_t>& newData, uint32_t newFlags = 0) override;

            virtual bool setData(const std::vector<uint8_t>& newData) override;

            MTLBufferPtr getBuffer() const { return buffer; }

        protected:
            bool createBuffer();

            RendererMetal* rendererMetal;

            MTLBufferPtr buffer = nil;
            uint32_t bufferSize = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
