// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendStateMetal.h"

namespace ouzel
{
    namespace graphics
    {
        static MTLBlendFactor getBlendFactor(BlendStateResource::BlendFactor blendFactor)
        {
            switch (blendFactor)
            {
                case BlendStateResource::BlendFactor::ZERO: return MTLBlendFactorZero;
                case BlendStateResource::BlendFactor::ONE: return MTLBlendFactorOne;
                case BlendStateResource::BlendFactor::SRC_COLOR: return MTLBlendFactorSourceColor;
                case BlendStateResource::BlendFactor::INV_SRC_COLOR: return MTLBlendFactorOneMinusSourceColor;
                case BlendStateResource::BlendFactor::SRC_ALPHA: return MTLBlendFactorSourceAlpha;
                case BlendStateResource::BlendFactor::INV_SRC_ALPHA: return MTLBlendFactorOneMinusSourceAlpha;
                case BlendStateResource::BlendFactor::DEST_ALPHA: return MTLBlendFactorDestinationAlpha;
                case BlendStateResource::BlendFactor::INV_DEST_ALPHA: return MTLBlendFactorOneMinusDestinationAlpha;
                case BlendStateResource::BlendFactor::DEST_COLOR: return MTLBlendFactorDestinationColor;
                case BlendStateResource::BlendFactor::INV_DEST_COLOR: return MTLBlendFactorOneMinusDestinationColor;
                case BlendStateResource::BlendFactor::SRC_ALPHA_SAT: return MTLBlendFactorSourceAlphaSaturated;
                case BlendStateResource::BlendFactor::BLEND_FACTOR: return MTLBlendFactorBlendColor;
                case BlendStateResource::BlendFactor::INV_BLEND_FACTOR: return MTLBlendFactorOneMinusBlendColor;
            }

            return MTLBlendFactorZero;
        }

        static MTLBlendOperation getBlendOperation(BlendStateResource::BlendOperation blendOperation)
        {
            switch (blendOperation)
            {
                case BlendStateResource::BlendOperation::ADD: return MTLBlendOperationAdd;
                case BlendStateResource::BlendOperation::SUBTRACT: return MTLBlendOperationSubtract;
                case BlendStateResource::BlendOperation::REV_SUBTRACT: return MTLBlendOperationReverseSubtract;
                case BlendStateResource::BlendOperation::MIN: return MTLBlendOperationMin;
                case BlendStateResource::BlendOperation::MAX: return MTLBlendOperationMax;
            }

            return MTLBlendOperationAdd;
        }

        BlendStateMetal::BlendStateMetal()
        {
        }

        bool BlendStateMetal::upload()
        {
            if (!BlendStateResource::upload())
            {
                return false;
            }

            rgbBlendOperation = getBlendOperation(uploadData.colorOperation);
            alphaBlendOperation = getBlendOperation(uploadData.alphaOperation);
            sourceRGBBlendFactor = getBlendFactor(uploadData.colorBlendSource);
            destinationRGBBlendFactor = getBlendFactor(uploadData.colorBlendDest);
            sourceAlphaBlendFactor = getBlendFactor(uploadData.alphaBlendSource);
            destinationAlphaBlendFactor = getBlendFactor(uploadData.alphaBlendDest);
            metalBlendingEnabled = uploadData.enableBlending;

            return true;
        }

    } // namespace graphics
} // namespace ouzel
