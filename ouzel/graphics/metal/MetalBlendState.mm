// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "MetalBlendState.hpp"
#include "MetalRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            static MTLBlendFactor getBlendFactor(ouzel::graphics::BlendState::Factor blendFactor)
            {
                switch (blendFactor)
                {
                    case ouzel::graphics::BlendState::Factor::ZERO: return MTLBlendFactorZero;
                    case ouzel::graphics::BlendState::Factor::ONE: return MTLBlendFactorOne;
                    case ouzel::graphics::BlendState::Factor::SRC_COLOR: return MTLBlendFactorSourceColor;
                    case ouzel::graphics::BlendState::Factor::INV_SRC_COLOR: return MTLBlendFactorOneMinusSourceColor;
                    case ouzel::graphics::BlendState::Factor::SRC_ALPHA: return MTLBlendFactorSourceAlpha;
                    case ouzel::graphics::BlendState::Factor::INV_SRC_ALPHA: return MTLBlendFactorOneMinusSourceAlpha;
                    case ouzel::graphics::BlendState::Factor::DEST_ALPHA: return MTLBlendFactorDestinationAlpha;
                    case ouzel::graphics::BlendState::Factor::INV_DEST_ALPHA: return MTLBlendFactorOneMinusDestinationAlpha;
                    case ouzel::graphics::BlendState::Factor::DEST_COLOR: return MTLBlendFactorDestinationColor;
                    case ouzel::graphics::BlendState::Factor::INV_DEST_COLOR: return MTLBlendFactorOneMinusDestinationColor;
                    case ouzel::graphics::BlendState::Factor::SRC_ALPHA_SAT: return MTLBlendFactorSourceAlphaSaturated;
                    case ouzel::graphics::BlendState::Factor::BLEND_FACTOR: return MTLBlendFactorBlendColor;
                    case ouzel::graphics::BlendState::Factor::INV_BLEND_FACTOR: return MTLBlendFactorOneMinusBlendColor;
                }

                return MTLBlendFactorZero;
            }

            static MTLBlendOperation getBlendOperation(ouzel::graphics::BlendState::Operation blendOperation)
            {
                switch (blendOperation)
                {
                    case ouzel::graphics::BlendState::Operation::ADD: return MTLBlendOperationAdd;
                    case ouzel::graphics::BlendState::Operation::SUBTRACT: return MTLBlendOperationSubtract;
                    case ouzel::graphics::BlendState::Operation::REV_SUBTRACT: return MTLBlendOperationReverseSubtract;
                    case ouzel::graphics::BlendState::Operation::MIN: return MTLBlendOperationMin;
                    case ouzel::graphics::BlendState::Operation::MAX: return MTLBlendOperationMax;
                }

                return MTLBlendOperationAdd;
            }

            BlendState::BlendState(RenderDevice& renderDevice,
                                   bool enableBlending,
                                   ouzel::graphics::BlendState::Factor colorBlendSource,
                                   ouzel::graphics::BlendState::Factor colorBlendDest,
                                   ouzel::graphics::BlendState::Operation colorOperation,
                                   ouzel::graphics::BlendState::Factor alphaBlendSource,
                                   ouzel::graphics::BlendState::Factor alphaBlendDest,
                                   ouzel::graphics::BlendState::Operation alphaOperation,
                                   uint8_t colorMask):
                RenderResource(renderDevice),
                rgbBlendOperation(getBlendOperation(colorOperation)),
                alphaBlendOperation(getBlendOperation(alphaOperation)),
                sourceRgbBlendFactor(getBlendFactor(colorBlendSource)),
                destinationRgbBlendFactor(getBlendFactor(colorBlendDest)),
                sourceAlphaBlendFactor(getBlendFactor(alphaBlendSource)),
                destinationAlphaBlendFactor(getBlendFactor(alphaBlendDest)),
                blendingEnabled(enableBlending)
            {
                colorWriteMask = MTLColorWriteMaskNone;
                if (colorMask & ouzel::graphics::BlendState::COLOR_MASK_RED) colorWriteMask |= MTLColorWriteMaskRed;
                if (colorMask & ouzel::graphics::BlendState::COLOR_MASK_GREEN) colorWriteMask |= MTLColorWriteMaskGreen;
                if (colorMask & ouzel::graphics::BlendState::COLOR_MASK_BLUE) colorWriteMask |= MTLColorWriteMaskBlue;
                if (colorMask & ouzel::graphics::BlendState::COLOR_MASK_ALPHA) colorWriteMask |= MTLColorWriteMaskAlpha;
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
