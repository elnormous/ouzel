// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALBUFFER_HPP
#define OUZEL_GRAPHICS_METALBUFFER_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#  import <Metal/Metal.h>
typedef id<MTLBuffer> MTLBufferPtr;
#else
#  include <objc/objc.h>
typedef id MTLBufferPtr;
#endif

#include "MetalRenderResource.hpp"
#include "MetalPointer.hpp"
#include "../BufferType.hpp"

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
                       std::uint32_t initFlags,
                       const std::vector<std::uint8_t>& initData,
                       std::uint32_t initSize);

                void setData(const std::vector<std::uint8_t>& data);

                auto getFlags() const noexcept { return flags; }
                auto getType() const noexcept { return type; }
                auto getSize() const noexcept { return size; }

                auto& getBuffer() const noexcept { return buffer; }

            private:
                void createBuffer(NSUInteger newSize);

                BufferType type;
                std::uint32_t flags = 0;

                Pointer<MTLBufferPtr> buffer;
                NSUInteger size = 0;
            };
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALBUFFER_HPP
