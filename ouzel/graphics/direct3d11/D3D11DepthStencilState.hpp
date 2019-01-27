// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11DEPTHSTENCILSTATE_HPP
#define OUZEL_GRAPHICS_D3D11DEPTHSTENCILSTATE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <d3d11.h>
#include "graphics/direct3d11/D3D11RenderResource.hpp"
#include "graphics/DepthStencilState.hpp"

namespace ouzel
{
    namespace graphics
    {
        class D3D11RenderDevice;

        class D3D11DepthStencilState final: public D3D11RenderResource
        {
        public:
            D3D11DepthStencilState(D3D11RenderDevice& renderDeviceD3D11,
                                   bool initDepthTest,
                                   bool initDepthWrite,
                                   DepthStencilState::CompareFunction initCompareFunction,
                                   uint32_t initStencilReadMask,
                                   uint32_t initStencilWriteMask);
            ~D3D11DepthStencilState();

            ID3D11DepthStencilState* getDepthStencilState() const { return depthStencilState; }

        private:
            ID3D11DepthStencilState* depthStencilState = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11DEPTHSTENCILSTATE_HPP
