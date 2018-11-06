// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef DEPTHSTENCILSTATERESOURCED3D11_HPP
#define DEPTHSTENCILSTATERESOURCED3D11_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_D3D11

#include <d3d11.h>
#include "graphics/direct3d11/RenderResourceD3D11.hpp"
#include "graphics/DepthStencilState.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceD3D11;

        class DepthStencilStateResourceD3D11 final: public RenderResourceD3D11
        {
        public:
            DepthStencilStateResourceD3D11(RenderDeviceD3D11& renderDeviceD3D11,
                                           bool initDepthTest,
                                           bool initDepthWrite,
                                           DepthStencilState::CompareFunction initCompareFunction);
            ~DepthStencilStateResourceD3D11();

            ID3D11DepthStencilState* getDepthStencilState() const { return depthStencilState; }

        private:
            ID3D11DepthStencilState* depthStencilState = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // DEPTHSTENCILSTATERESOURCED3D11_HPP
