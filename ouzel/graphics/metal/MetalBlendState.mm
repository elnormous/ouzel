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
            constexpr MTLBlendFactor getBlendFactor(BlendFactor blendFactor)
            {
                switch (blendFactor)
                {
                    case BlendFactor::ZERO: return MTLBlendFactorZero;
                    case BlendFactor::ONE: return MTLBlendFactorOne;
                    case BlendFactor::SRC_COLOR: return MTLBlendFactorSourceColor;
                    case BlendFactor::INV_SRC_COLOR: return MTLBlendFactorOneMinusSourceColor;
                    case BlendFactor::SRC_ALPHA: return MTLBlendFactorSourceAlpha;
                    case BlendFactor::INV_SRC_ALPHA: return MTLBlendFactorOneMinusSourceAlpha;
                    case BlendFactor::DEST_ALPHA: return MTLBlendFactorDestinationAlpha;
                    case BlendFactor::INV_DEST_ALPHA: return MTLBlendFactorOneMinusDestinationAlpha;
                    case BlendFactor::DEST_COLOR: return MTLBlendFactorDestinationColor;
                    case BlendFactor::INV_DEST_COLOR: return MTLBlendFactorOneMinusDestinationColor;
                    case BlendFactor::SRC_ALPHA_SAT: return MTLBlendFactorSourceAlphaSaturated;
                    case BlendFactor::BLEND_FACTOR: return MTLBlendFactorBlendColor;
                    case BlendFactor::INV_BLEND_FACTOR: return MTLBlendFactorOneMinusBlendColor;
                }

                return MTLBlendFactorZero;
            }

            constexpr MTLBlendOperation getBlendOperation(BlendOperation blendOperation)
            {
                switch (blendOperation)
                {
                    case BlendOperation::ADD: return MTLBlendOperationAdd;
                    case BlendOperation::SUBTRACT: return MTLBlendOperationSubtract;
                    case BlendOperation::REV_SUBTRACT: return MTLBlendOperationReverseSubtract;
                    case BlendOperation::MIN: return MTLBlendOperationMin;
                    case BlendOperation::MAX: return MTLBlendOperationMax;
                }

                return MTLBlendOperationAdd;
            }

            BlendState::BlendState(RenderDevice& initRenderDevice,
                                   bool enableBlending,
                                   BlendFactor colorBlendSource,
                                   BlendFactor colorBlendDest,
                                   BlendOperation colorOperation,
                                   BlendFactor alphaBlendSource,
                                   BlendFactor alphaBlendDest,
                                   BlendOperation alphaOperation,
                                   uint8_t colorMask):
                RenderResource(initRenderDevice),
                rgbBlendOperation(getBlendOperation(colorOperation)),
                alphaBlendOperation(getBlendOperation(alphaOperation)),
                sourceRgbBlendFactor(getBlendFactor(colorBlendSource)),
                destinationRgbBlendFactor(getBlendFactor(colorBlendDest)),
                sourceAlphaBlendFactor(getBlendFactor(alphaBlendSource)),
                destinationAlphaBlendFactor(getBlendFactor(alphaBlendDest)),
                blendingEnabled(enableBlending)
            {
                colorWriteMask = MTLColorWriteMaskNone;
                if (colorMask & ColorMask::COLOR_MASK_RED) colorWriteMask |= MTLColorWriteMaskRed;
                if (colorMask & ColorMask::COLOR_MASK_GREEN) colorWriteMask |= MTLColorWriteMaskGreen;
                if (colorMask & ColorMask::COLOR_MASK_BLUE) colorWriteMask |= MTLColorWriteMaskBlue;
                if (colorMask & ColorMask::COLOR_MASK_ALPHA) colorWriteMask |= MTLColorWriteMaskAlpha;
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
