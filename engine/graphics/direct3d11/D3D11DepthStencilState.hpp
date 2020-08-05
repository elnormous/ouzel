// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11DEPTHSTENCILSTATE_HPP
#define OUZEL_GRAPHICS_D3D11DEPTHSTENCILSTATE_HPP

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
#include "../CompareFunction.hpp"
#include "../StencilOperation.hpp"

namespace ouzel::graphics::d3d11
{
    class RenderDevice;

    class DepthStencilState final: public RenderResource
    {
    public:
        DepthStencilState(RenderDevice& initRenderDevice,
                          bool initDepthTest,
                          bool initDepthWrite,
                          CompareFunction initCompareFunction,
                          bool initStencilEnabled,
                          std::uint32_t initStencilReadMask,
                          std::uint32_t initStencilWriteMask,
                          StencilOperation initFrontFaceStencilFailureOperation,
                          StencilOperation initFrontFaceStencilDepthFailureOperation,
                          StencilOperation initFrontFaceStencilPassOperation,
                          CompareFunction initFrontFaceStencilCompareFunction,
                          StencilOperation initBackFaceStencilFailureOperation,
                          StencilOperation initBackFaceStencilDepthFailureOperation,
                          StencilOperation initBackFaceStencilPassOperation,
                          CompareFunction initBackFaceStencilCompareFunction);

        auto& getDepthStencilState() const noexcept { return depthStencilState; }

    private:
        Pointer<ID3D11DepthStencilState> depthStencilState;
    };
}
#endif

#endif // OUZEL_GRAPHICS_D3D11DEPTHSTENCILSTATE_HPP
