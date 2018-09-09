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

        class BlendStateResourceD3D11: public RenderResource
        {
        public:
            explicit BlendStateResourceD3D11(RenderDeviceD3D11& renderDeviceD3D11);
            virtual ~BlendStateResourceD3D11();

            void init(bool newEnableBlending,
                      BlendState::Factor newColorBlendSource, BlendState::Factor newColorBlendDest,
                      BlendState::Operation newColorOperation,
                      BlendState::Factor newAlphaBlendSource, BlendState::Factor newAlphaBlendDest,
                      BlendState::Operation newAlphaOperation,
                      uint8_t newColorMask);

            ID3D11BlendState* getBlendState() const { return blendState; }

        private:
            BlendState::Factor colorBlendSource = BlendState::Factor::ONE;
            BlendState::Factor colorBlendDest = BlendState::Factor::ZERO;
            BlendState::Operation colorOperation = BlendState::Operation::ADD;
            BlendState::Factor alphaBlendSource = BlendState::Factor::ONE;
            BlendState::Factor alphaBlendDest = BlendState::Factor::ZERO;
            BlendState::Operation alphaOperation = BlendState::Operation::ADD;
            uint8_t colorMask = BlendState::COLOR_MASK_ALL;
            bool enableBlending = false;

            ID3D11BlendState* blendState = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif
