// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendStateMetal.h"

namespace ouzel
{
    namespace graphics
    {
        BlendStateMetal::BlendStateMetal()
        {

        }

        BlendStateMetal::~BlendStateMetal()
        {

        }

        bool BlendStateMetal::init(bool newEnableBlending,
                                   BlendFactor newColorBlendSource, BlendFactor newColorBlendDest,
                                   BlendOperation newColorOperation,
                                   BlendFactor newAlphaBlendSource, BlendFactor newAlphaBlendDest,
                                   BlendOperation newAlphaOperation)
        {
            if (!BlendState::init(newEnableBlending,
                                  newColorBlendSource,
                                  newColorBlendDest,
                                  newColorOperation,
                                  newAlphaBlendSource,
                                  newAlphaBlendDest,
                                  newAlphaOperation))
            {
                return false;
            }

            rgbBlendOperation = getBlendOperation(newColorOperation);
            alphaBlendOperation = getBlendOperation(newAlphaOperation);
            sourceRGBBlendFactor = getBlendFactor(newColorBlendSource);
            destinationRGBBlendFactor = getBlendFactor(newColorBlendDest);
            sourceAlphaBlendFactor = getBlendFactor(newAlphaBlendSource);
            destinationAlphaBlendFactor = getBlendFactor(newAlphaBlendDest);

            return true;
        }

        MTLBlendFactor BlendStateMetal::getBlendFactor(BlendFactor blendFactor)
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

        MTLBlendOperation BlendStateMetal::getBlendOperation(BlendOperation blendOperation)
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

    } // namespace graphics
} // namespace ouzel
