// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11BLENDSTATE_HPP
#define OUZEL_GRAPHICS_D3D11BLENDSTATE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <d3d11.h>
#include "graphics/direct3d11/D3D11RenderResource.hpp"
#include "graphics/BlendState.hpp"

namespace ouzel
{
    namespace graphics
    {
        class D3D11RenderDevice;

        class D3D11BlendState final: public D3D11RenderResource
        {
        public:
            D3D11BlendState(D3D11RenderDevice& renderDeviceD3D11,
                            bool enableBlending,
                            BlendState::Factor colorBlendSource, BlendState::Factor colorBlendDest,
                            BlendState::Operation colorOperation,
                            BlendState::Factor alphaBlendSource, BlendState::Factor alphaBlendDest,
                            BlendState::Operation alphaOperation,
                            uint8_t colorMask);
            ~D3D11BlendState();

            ID3D11BlendState* getBlendState() const { return blendState; }

        private:
            ID3D11BlendState* blendState = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11BLENDSTATE_HPP
