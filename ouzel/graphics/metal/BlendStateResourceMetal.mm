// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#include "BlendStateResourceMetal.h"

namespace ouzel
{
    namespace graphics
    {
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

        BlendStateResourceMetal::BlendStateResourceMetal()
        {
        }

        bool BlendStateResourceMetal::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
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

                dirty = 0;
            }

            return true;
        }

    } // namespace graphics
} // namespace ouzel

#endif
