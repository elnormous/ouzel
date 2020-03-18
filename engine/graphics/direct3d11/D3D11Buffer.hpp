// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11BUFFER_HPP
#define OUZEL_GRAPHICS_D3D11BUFFER_HPP

#include "core/Setup.h"

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

#include "graphics/direct3d11/D3D11RenderResource.hpp"
#include "graphics/direct3d11/D3D11Pointer.hpp"
#include "graphics/BufferType.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
        {
            class RenderDevice;

            class Buffer final: public RenderResource
            {
            public:
                Buffer(RenderDevice& initRenderDevice,
                       BufferType initType,
                       std::uint32_t initFlags,
                       const std::vector<std::uint8_t>& data,
                       std::uint32_t initSize);

                void setData(const std::vector<std::uint8_t>& data);

                inline auto getFlags() const noexcept { return flags; }
                inline auto getType() const noexcept { return type; }
                inline auto getSize() const noexcept { return size; }

                inline auto& getBuffer() const noexcept { return buffer; }

            private:
                void createBuffer(UINT newSize, const std::vector<std::uint8_t>& data);

                BufferType type;
                std::uint32_t flags = 0;

                Pointer<ID3D11Buffer> buffer;
                UINT size = 0;
            };
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11BUFFER_HPP
