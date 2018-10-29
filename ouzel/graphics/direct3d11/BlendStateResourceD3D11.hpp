// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <d3d11.h>
#include "graphics/RenderResource.hpp"
#include "graphics/BlendState.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceD3D11;

        class BlendStateResourceD3D11 final: public RenderResource
        {
        public:
            explicit BlendStateResourceD3D11(RenderDeviceD3D11& renderDeviceD3D11);
            ~BlendStateResourceD3D11();

            void init(bool enableBlending,
                      BlendState::Factor colorBlendSource, BlendState::Factor colorBlendDest,
                      BlendState::Operation colorOperation,
                      BlendState::Factor alphaBlendSource, BlendState::Factor alphaBlendDest,
                      BlendState::Operation alphaOperation,
                      uint8_t colorMask);

            ID3D11BlendState* getBlendState() const { return blendState; }

        private:
            ID3D11BlendState* blendState = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif
