// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_BLENDSTATED3D11_HPP
#define OUZEL_GRAPHICS_BLENDSTATED3D11_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <d3d11.h>
#include "graphics/direct3d11/RenderResourceD3D11.hpp"
#include "graphics/BlendState.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceD3D11;

        class BlendStateD3D11 final: public RenderResourceD3D11
        {
        public:
            BlendStateD3D11(RenderDeviceD3D11& renderDeviceD3D11,
                            bool enableBlending,
                            BlendState::Factor colorBlendSource, BlendState::Factor colorBlendDest,
                            BlendState::Operation colorOperation,
                            BlendState::Factor alphaBlendSource, BlendState::Factor alphaBlendDest,
                            BlendState::Operation alphaOperation,
                            uint8_t colorMask);
            ~BlendStateD3D11();

            ID3D11BlendState* getBlendState() const { return blendState; }

        private:
            ID3D11BlendState* blendState = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_BLENDSTATED3D11_HPP
