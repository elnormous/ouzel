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
            static MTLBlendFactor getBlendFactor(graphics::BlendState::Factor blendFactor)
            {
                switch (blendFactor)
                {
                    case graphics::BlendState::Factor::ZERO: return MTLBlendFactorZero;
                    case graphics::BlendState::Factor::ONE: return MTLBlendFactorOne;
                    case graphics::BlendState::Factor::SRC_COLOR: return MTLBlendFactorSourceColor;
                    case graphics::BlendState::Factor::INV_SRC_COLOR: return MTLBlendFactorOneMinusSourceColor;
                    case graphics::BlendState::Factor::SRC_ALPHA: return MTLBlendFactorSourceAlpha;
                    case graphics::BlendState::Factor::INV_SRC_ALPHA: return MTLBlendFactorOneMinusSourceAlpha;
                    case graphics::BlendState::Factor::DEST_ALPHA: return MTLBlendFactorDestinationAlpha;
                    case graphics::BlendState::Factor::INV_DEST_ALPHA: return MTLBlendFactorOneMinusDestinationAlpha;
                    case graphics::BlendState::Factor::DEST_COLOR: return MTLBlendFactorDestinationColor;
                    case graphics::BlendState::Factor::INV_DEST_COLOR: return MTLBlendFactorOneMinusDestinationColor;
                    case graphics::BlendState::Factor::SRC_ALPHA_SAT: return MTLBlendFactorSourceAlphaSaturated;
                    case graphics::BlendState::Factor::BLEND_FACTOR: return MTLBlendFactorBlendColor;
                    case graphics::BlendState::Factor::INV_BLEND_FACTOR: return MTLBlendFactorOneMinusBlendColor;
                }

                return MTLBlendFactorZero;
            }

            static MTLBlendOperation getBlendOperation(graphics::BlendState::Operation blendOperation)
            {
                switch (blendOperation)
                {
                    case graphics::BlendState::Operation::ADD: return MTLBlendOperationAdd;
                    case graphics::BlendState::Operation::SUBTRACT: return MTLBlendOperationSubtract;
                    case graphics::BlendState::Operation::REV_SUBTRACT: return MTLBlendOperationReverseSubtract;
                    case graphics::BlendState::Operation::MIN: return MTLBlendOperationMin;
                    case graphics::BlendState::Operation::MAX: return MTLBlendOperationMax;
                }

                return MTLBlendOperationAdd;
            }

            BlendState::BlendState(RenderDevice& renderDevice,
                                   bool enableBlending,
                                   graphics::BlendState::Factor colorBlendSource,
                                   graphics::BlendState::Factor colorBlendDest,
                                   graphics::BlendState::Operation colorOperation,
                                   graphics::BlendState::Factor alphaBlendSource,
                                   graphics::BlendState::Factor alphaBlendDest,
                                   graphics::BlendState::Operation alphaOperation,
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
                if (colorMask & graphics::BlendState::COLOR_MASK_RED) colorWriteMask |= MTLColorWriteMaskRed;
                if (colorMask & graphics::BlendState::COLOR_MASK_GREEN) colorWriteMask |= MTLColorWriteMaskGreen;
                if (colorMask & graphics::BlendState::COLOR_MASK_BLUE) colorWriteMask |= MTLColorWriteMaskBlue;
                if (colorMask & graphics::BlendState::COLOR_MASK_ALPHA) colorWriteMask |= MTLColorWriteMaskAlpha;
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
