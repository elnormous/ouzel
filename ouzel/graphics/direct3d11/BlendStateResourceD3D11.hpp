// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_BLENDSTATERESOURCED3D11_HPP
#define OUZEL_BLENDSTATERESOURCED3D11_HPP

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

        class BlendStateResourceD3D11 final: public RenderResourceD3D11
        {
        public:
            BlendStateResourceD3D11(RenderDeviceD3D11& renderDeviceD3D11,
                                    bool enableBlending,
                                    BlendState::Factor colorBlendSource, BlendState::Factor colorBlendDest,
                                    BlendState::Operation colorOperation,
                                    BlendState::Factor alphaBlendSource, BlendState::Factor alphaBlendDest,
                                    BlendState::Operation alphaOperation,
                                    uint8_t colorMask);
            ~BlendStateResourceD3D11();

            ID3D11BlendState* getBlendState() const { return blendState; }

        private:
            ID3D11BlendState* blendState = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_BLENDSTATERESOURCED3D11_HPP
