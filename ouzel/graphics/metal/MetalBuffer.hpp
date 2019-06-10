// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALBUFFER_HPP
#define OUZEL_GRAPHICS_METALBUFFER_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#  import <Metal/Metal.h>
typedef id<MTLBuffer> MTLBufferPtr;
#else
#  include <objc/objc.h>
typedef id MTLBufferPtr;
#endif

#include "graphics/metal/MetalRenderResource.hpp"
#include "graphics/Buffer.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            class RenderDevice;

            class Buffer final: public RenderResource
            {
            public:
                Buffer(RenderDevice& renderDevice,
                       graphics::Buffer::Usage newUsage, uint32_t newFlags,
                       const std::vector<uint8_t>& newData,
                       uint32_t newSize);
                ~Buffer();

                void setData(const std::vector<uint8_t>& data);

                inline uint32_t getFlags() const { return flags; }
                inline graphics::Buffer::Usage getUsage() const { return usage; }
                inline NSUInteger getSize() const { return size; }

                inline MTLBufferPtr getBuffer() const { return buffer; }

            private:
                void createBuffer(NSUInteger newSize);

                graphics::Buffer::Usage usage;
                uint32_t flags = 0;

                MTLBufferPtr buffer = nil;
                NSUInteger size = 0;
            };
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALBUFFER_HPP
