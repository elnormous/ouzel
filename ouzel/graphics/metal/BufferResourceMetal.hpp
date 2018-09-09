// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#  import <Metal/Metal.h>
typedef id<MTLBuffer> MTLBufferPtr;
#else
#  include <objc/objc.h>
typedef id MTLBufferPtr;
#endif

#include "graphics/RenderResource.hpp"
#include "graphics/Buffer.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class BufferResourceMetal: public RenderResource
        {
        public:
            explicit BufferResourceMetal(RenderDeviceMetal& renderDeviceMetal);
            virtual ~BufferResourceMetal();

            void init(Buffer::Usage newUsage, uint32_t newFlags,
                      const std::vector<uint8_t>& newData,
                      uint32_t newSize);

            void setData(const std::vector<uint8_t>& newData);

            inline uint32_t getFlags() const { return flags; }
            inline Buffer::Usage getUsage() const { return usage; }
            inline uint32_t getSize() const { return static_cast<uint32_t>(data.size()); }

            inline MTLBufferPtr getBuffer() const { return buffer; }

        private:
            void createBuffer(NSUInteger newSize);

            Buffer::Usage usage;
            uint32_t flags = 0;
            std::vector<uint8_t> data;

            MTLBufferPtr buffer = nil;
            NSUInteger bufferSize = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
