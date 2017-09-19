// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "BlendStateResourceMetal.hpp"

namespace ouzel
{
    namespace graphics
    {
        BlendStateResourceMetal::BlendStateResourceMetal()
        {
        }

        static MTLBlendFactor getBlendFactor(BlendState::BlendFactor blendFactor)
        {
            switch (blendFactor)
            {
                case BlendState::BlendFactor::ZERO: return MTLBlendFactorZero;
                case BlendState::BlendFactor::ONE: return MTLBlendFactorOne;
                case BlendState::BlendFactor::SRC_COLOR: return MTLBlendFactorSourceColor;
                case BlendState::BlendFactor::INV_SRC_COLOR: return MTLBlendFactorOneMinusSourceColor;
                case BlendState::BlendFactor::SRC_ALPHA: return MTLBlendFactorSourceAlpha;
                case BlendState::BlendFactor::INV_SRC_ALPHA: return MTLBlendFactorOneMinusSourceAlpha;
                case BlendState::BlendFactor::DEST_ALPHA: return MTLBlendFactorDestinationAlpha;
                case BlendState::BlendFactor::INV_DEST_ALPHA: return MTLBlendFactorOneMinusDestinationAlpha;
                case BlendState::BlendFactor::DEST_COLOR: return MTLBlendFactorDestinationColor;
                case BlendState::BlendFactor::INV_DEST_COLOR: return MTLBlendFactorOneMinusDestinationColor;
                case BlendState::BlendFactor::SRC_ALPHA_SAT: return MTLBlendFactorSourceAlphaSaturated;
                case BlendState::BlendFactor::BLEND_FACTOR: return MTLBlendFactorBlendColor;
                case BlendState::BlendFactor::INV_BLEND_FACTOR: return MTLBlendFactorOneMinusBlendColor;
            }

            return MTLBlendFactorZero;
        }

        static MTLBlendOperation getBlendOperation(BlendState::BlendOperation blendOperation)
        {
            switch (blendOperation)
            {
                case BlendState::BlendOperation::ADD: return MTLBlendOperationAdd;
                case BlendState::BlendOperation::SUBTRACT: return MTLBlendOperationSubtract;
                case BlendState::BlendOperation::REV_SUBTRACT: return MTLBlendOperationReverseSubtract;
                case BlendState::BlendOperation::MIN: return MTLBlendOperationMin;
                case BlendState::BlendOperation::MAX: return MTLBlendOperationMax;
            }

            return MTLBlendOperationAdd;
        }

        bool BlendStateResourceMetal::init(bool newEnableBlending,
                                           BlendState::BlendFactor newColorBlendSource, BlendState::BlendFactor newColorBlendDest,
                                           BlendState::BlendOperation newColorOperation,
                                           BlendState::BlendFactor newAlphaBlendSource, BlendState::BlendFactor newAlphaBlendDest,
                                           BlendState::BlendOperation newAlphaOperation,
                                           uint8_t newColorMask)
        {
            if (!BlendStateResource::init(newEnableBlending,
                                          newColorBlendSource, newColorBlendDest,
                                          newColorOperation,
                                          newAlphaBlendSource, newAlphaBlendDest,
                                          newAlphaOperation,
                                          newColorMask))
            {
                return false;
            }

            rgbBlendOperation = getBlendOperation(colorOperation);
            alphaBlendOperation = getBlendOperation(alphaOperation);
            sourceRGBBlendFactor = getBlendFactor(colorBlendSource);
            destinationRGBBlendFactor = getBlendFactor(colorBlendDest);
            sourceAlphaBlendFactor = getBlendFactor(alphaBlendSource);
            destinationAlphaBlendFactor = getBlendFactor(alphaBlendDest);
            metalBlendingEnabled = enableBlending;

            colorWriteMask = MTLColorWriteMaskNone;
            if (colorMask & BlendState::COLOR_MASK_RED) colorWriteMask |= MTLColorWriteMaskRed;
            if (colorMask & BlendState::COLOR_MASK_GREEN) colorWriteMask |= MTLColorWriteMaskGreen;
            if (colorMask & BlendState::COLOR_MASK_BLUE) colorWriteMask |= MTLColorWriteMaskBlue;
            if (colorMask & BlendState::COLOR_MASK_ALPHA) colorWriteMask |= MTLColorWriteMaskAlpha;

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
