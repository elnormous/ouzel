// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_METALBUFFER_HPP
#define OUZEL_GRAPHICS_METALBUFFER_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#ifdef __OBJC__
#  import <Metal/Metal.h>
using MTLBufferPtr = id<MTLBuffer>;
#else
#  include <objc/objc.h>
using MTLBufferPtr = id;
#endif

#include "MetalRenderResource.hpp"
#include "../BufferType.hpp"
#include "../Flags.hpp"
#include "../../platform/objc/Pointer.hpp"

namespace ouzel::graphics::metal
{
    class RenderDevice;

    class Buffer final: public RenderResource
    {
    public:
        Buffer(RenderDevice& initRenderDevice,
               BufferType initType,
               Flags initFlags,
               const std::vector<std::uint8_t>& initData,
               std::uint32_t initSize);

        void setData(const std::vector<std::uint8_t>& data);

        auto getFlags() const noexcept { return flags; }
        auto getType() const noexcept { return type; }
        auto getSize() const noexcept { return size; }

        auto& getBuffer() const noexcept { return buffer; }

    private:
        void createBuffer(NSUInteger newSize);

        BufferType type = BufferType::index;
        Flags flags = Flags::none;

        platform::objc::Pointer<MTLBufferPtr> buffer;
        NSUInteger size = 0;
    };
}

#endif

#endif // OUZEL_GRAPHICS_METALBUFFER_HPP
