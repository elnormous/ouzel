// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

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
            namespace
            {
                constexpr MTLBlendFactor getBlendFactor(BlendFactor blendFactor)
                {
                    switch (blendFactor)
                    {
                        case BlendFactor::Zero: return MTLBlendFactorZero;
                        case BlendFactor::One: return MTLBlendFactorOne;
                        case BlendFactor::SrcColor: return MTLBlendFactorSourceColor;
                        case BlendFactor::InvSrcColor: return MTLBlendFactorOneMinusSourceColor;
                        case BlendFactor::SrcAlpha: return MTLBlendFactorSourceAlpha;
                        case BlendFactor::InvSrcAlpha: return MTLBlendFactorOneMinusSourceAlpha;
                        case BlendFactor::DestAlpha: return MTLBlendFactorDestinationAlpha;
                        case BlendFactor::InvDestAlpha: return MTLBlendFactorOneMinusDestinationAlpha;
                        case BlendFactor::DestColor: return MTLBlendFactorDestinationColor;
                        case BlendFactor::InvDestColor: return MTLBlendFactorOneMinusDestinationColor;
                        case BlendFactor::SrcAlphaSat: return MTLBlendFactorSourceAlphaSaturated;
                        case BlendFactor::BlendFactor: return MTLBlendFactorBlendColor;
                        case BlendFactor::InvBlendFactor: return MTLBlendFactorOneMinusBlendColor;
                    }

                    return MTLBlendFactorZero;
                }

                constexpr MTLBlendOperation getBlendOperation(BlendOperation blendOperation)
                {
                    switch (blendOperation)
                    {
                        case BlendOperation::Add: return MTLBlendOperationAdd;
                        case BlendOperation::Subtract: return MTLBlendOperationSubtract;
                        case BlendOperation::RevSubtract: return MTLBlendOperationReverseSubtract;
                        case BlendOperation::Min: return MTLBlendOperationMin;
                        case BlendOperation::Max: return MTLBlendOperationMax;
                    }

                    return MTLBlendOperationAdd;
                }
            }

            BlendState::BlendState(RenderDevice& initRenderDevice,
                                   bool enableBlending,
                                   BlendFactor colorBlendSource,
                                   BlendFactor colorBlendDest,
                                   BlendOperation colorOperation,
                                   BlendFactor alphaBlendSource,
                                   BlendFactor alphaBlendDest,
                                   BlendOperation alphaOperation,
                                   ColorMask colorMask):
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
                if ((colorMask & ColorMask::Red) == ColorMask::Red) colorWriteMask |= MTLColorWriteMaskRed;
                if ((colorMask & ColorMask::Green) == ColorMask::Green) colorWriteMask |= MTLColorWriteMaskGreen;
                if ((colorMask & ColorMask::Blue) == ColorMask::Blue) colorWriteMask |= MTLColorWriteMaskBlue;
                if ((colorMask & ColorMask::Alpha) == ColorMask::Alpha) colorWriteMask |= MTLColorWriteMaskAlpha;
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
