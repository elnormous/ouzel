// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11BLENDSTATE_HPP
#define OUZEL_GRAPHICS_D3D11BLENDSTATE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <d3d11.h>
#include "graphics/direct3d11/D3D11RenderResource.hpp"
#include "graphics/BlendFactor.hpp"
#include "graphics/BlendOperation.hpp"
#include "graphics/ColorMask.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
        {
            class RenderDevice;

            class BlendState final: public RenderResource
            {
            public:
                BlendState(RenderDevice& renderDevice,
                           bool enableBlending,
                           BlendFactor colorBlendSource,
                           BlendFactor colorBlendDest,
                           BlendOperation colorOperation,
                           BlendFactor alphaBlendSource,
                           BlendFactor alphaBlendDest,
                           BlendOperation alphaOperation,
                           uint8_t colorMask);
                ~BlendState();

                inline ID3D11BlendState* getBlendState() const { return blendState; }

            private:
                ID3D11BlendState* blendState = nullptr;
            };
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11BLENDSTATE_HPP
