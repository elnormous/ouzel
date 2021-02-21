// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11BLENDSTATE_HPP
#define OUZEL_GRAPHICS_D3D11BLENDSTATE_HPP

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
#include "../BlendFactor.hpp"
#include "../BlendOperation.hpp"
#include "../ColorMask.hpp"

namespace ouzel::graphics::d3d11
{
    class RenderDevice;

    class BlendState final: public RenderResource
    {
    public:
        BlendState(RenderDevice& initRenderDevice,
                   bool enableBlending,
                   BlendFactor colorBlendSource,
                   BlendFactor colorBlendDest,
                   BlendOperation colorOperation,
                   BlendFactor alphaBlendSource,
                   BlendFactor alphaBlendDest,
                   BlendOperation alphaOperation,
                   ColorMask colorMask);

        auto& getBlendState() const noexcept { return blendState; }

    private:
        Pointer<ID3D11BlendState> blendState;
    };
}
#endif

#endif // OUZEL_GRAPHICS_D3D11BLENDSTATE_HPP
