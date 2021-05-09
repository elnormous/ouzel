// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "MetalBlendState.hpp"

namespace ouzel::graphics::metal
{
    namespace
    {
        constexpr MTLBlendFactor getBlendFactor(BlendFactor blendFactor)
        {
            switch (blendFactor)
            {
                case BlendFactor::zero: return MTLBlendFactorZero;
                case BlendFactor::one: return MTLBlendFactorOne;
                case BlendFactor::srcColor: return MTLBlendFactorSourceColor;
                case BlendFactor::invSrcColor: return MTLBlendFactorOneMinusSourceColor;
                case BlendFactor::srcAlpha: return MTLBlendFactorSourceAlpha;
                case BlendFactor::invSrcAlpha: return MTLBlendFactorOneMinusSourceAlpha;
                case BlendFactor::destAlpha: return MTLBlendFactorDestinationAlpha;
                case BlendFactor::invDestAlpha: return MTLBlendFactorOneMinusDestinationAlpha;
                case BlendFactor::destColor: return MTLBlendFactorDestinationColor;
                case BlendFactor::invDestColor: return MTLBlendFactorOneMinusDestinationColor;
                case BlendFactor::srcAlphaSat: return MTLBlendFactorSourceAlphaSaturated;
                case BlendFactor::blendFactor: return MTLBlendFactorBlendColor;
                case BlendFactor::invBlendFactor: return MTLBlendFactorOneMinusBlendColor;
                default: throw std::runtime_error("Unsupported blend factor");
            }
        }

        constexpr MTLBlendOperation getBlendOperation(BlendOperation blendOperation)
        {
            switch (blendOperation)
            {
                case BlendOperation::add: return MTLBlendOperationAdd;
                case BlendOperation::subtract: return MTLBlendOperationSubtract;
                case BlendOperation::reverseSubtract: return MTLBlendOperationReverseSubtract;
                case BlendOperation::min: return MTLBlendOperationMin;
                case BlendOperation::max: return MTLBlendOperationMax;
                default: throw std::runtime_error("Unsupported blend operation");
            }
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
        RenderResource{initRenderDevice},
        rgbBlendOperation{getBlendOperation(colorOperation)},
        alphaBlendOperation{getBlendOperation(alphaOperation)},
        sourceRgbBlendFactor{getBlendFactor(colorBlendSource)},
        destinationRgbBlendFactor{getBlendFactor(colorBlendDest)},
        sourceAlphaBlendFactor{getBlendFactor(alphaBlendSource)},
        destinationAlphaBlendFactor{getBlendFactor(alphaBlendDest)},
        blendingEnabled{enableBlending}
    {
        colorWriteMask = MTLColorWriteMaskNone;
        if ((colorMask & ColorMask::red) == ColorMask::red)
            colorWriteMask |= MTLColorWriteMaskRed;
        if ((colorMask & ColorMask::green) == ColorMask::green)
            colorWriteMask |= MTLColorWriteMaskGreen;
        if ((colorMask & ColorMask::blue) == ColorMask::blue)
            colorWriteMask |= MTLColorWriteMaskBlue;
        if ((colorMask & ColorMask::alpha) == ColorMask::alpha)
            colorWriteMask |= MTLColorWriteMaskAlpha;
    }
}

#endif
