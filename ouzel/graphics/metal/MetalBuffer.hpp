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
#include "graphics/BufferType.hpp"

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
                Buffer(RenderDevice& initRenderDevice,
                       BufferType initType,
                       uint32_t initFlags,
                       const std::vector<uint8_t>& initData,
                       uint32_t initSize);
                ~Buffer();

                void setData(const std::vector<uint8_t>& data);

                inline auto getFlags() const noexcept { return flags; }
                inline auto getType() const noexcept { return type; }
                inline auto getSize() const noexcept { return size; }

                inline auto getBuffer() const noexcept { return buffer; }

            private:
                void createBuffer(NSUInteger newSize);

                BufferType type;
                uint32_t flags = 0;

                MTLBufferPtr buffer = nil;
                NSUInteger size = 0;
            };
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALBUFFER_HPP
