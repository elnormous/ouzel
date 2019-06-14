// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
        namespace d3d11
        {
            class RenderDevice;

            class DepthStencilState final: public RenderResource
            {
            public:
                DepthStencilState(RenderDevice& renderDevice,
                                  bool initDepthTest,
                                  bool initDepthWrite,
                                  CompareFunction initCompareFunction,
                                  bool initStencilEnabled,
                                  uint32_t initStencilReadMask,
                                  uint32_t initStencilWriteMask,
                                  StencilOperation initFrontFaceStencilFailureOperation,
                                  StencilOperation initFrontFaceStencilDepthFailureOperation,
                                  StencilOperation initFrontFaceStencilPassOperation,
                                  CompareFunction initFrontFaceStencilCompareFunction,
                                  StencilOperation initBackFaceStencilFailureOperation,
                                  StencilOperation initBackFaceStencilDepthFailureOperation,
                                  StencilOperation initBackFaceStencilPassOperation,
                                  CompareFunction initBackFaceStencilCompareFunction);
                ~DepthStencilState();

                inline ID3D11DepthStencilState* getDepthStencilState() const { return depthStencilState; }

            private:
                ID3D11DepthStencilState* depthStencilState = nullptr;
            };
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11DEPTHSTENCILSTATE_HPP
