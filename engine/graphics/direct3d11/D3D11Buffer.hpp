// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11BUFFER_HPP
#define OUZEL_GRAPHICS_D3D11BUFFER_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <d3d11.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "D3D11RenderResource.hpp"
#include "D3D11Pointer.hpp"
#include "../BufferType.hpp"
#include "../Flags.hpp"

namespace ouzel::graphics::d3d11
{
    class RenderDevice;

    class Buffer final: public RenderResource
    {
    public:
        Buffer(RenderDevice& initRenderDevice,
               BufferType initType,
               Flags initFlags,
               const std::vector<std::uint8_t>& data,
               std::uint32_t initSize);

        void setData(const std::vector<std::uint8_t>& data);

        auto getFlags() const noexcept { return flags; }
        auto getType() const noexcept { return type; }
        auto getSize() const noexcept { return size; }

        auto& getBuffer() const noexcept { return buffer; }

    private:
        void createBuffer(UINT newSize, const std::vector<std::uint8_t>& data);

        BufferType type;
        Flags flags = Flags::none;

        Pointer<ID3D11Buffer> buffer;
        UINT size = 0;
    };
}
#endif

#endif // OUZEL_GRAPHICS_D3D11BUFFER_HPP
