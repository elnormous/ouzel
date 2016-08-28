// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"
#include "graphics/BlendState.h"

#if defined(__OBJC__)
#import <Metal/Metal.h>
#else
#include <objc/objc.h>
typedef NSUInteger MTLBlendFactor;
typedef NSUInteger MTLBlendOperation;
#endif

namespace ouzel
{
    namespace graphics
    {
        class RendererMetal;

        class BlendStateMetal: public BlendState
        {
            friend RendererMetal;
        public:
            virtual bool init(bool newEnableBlending,
                              BlendFactor newColorBlendSource, BlendFactor newColorBlendDest,
                              BlendOperation newColorOperation,
                              BlendFactor newAlphaBlendSource, BlendFactor newAlphaBlendDest,
                              BlendOperation newAlphaOperation) override;

            MTLBlendOperation getRGBBlendOperation() const { return rgbBlendOperation; }
            MTLBlendOperation getAlphaBlendOperation() const { return alphaBlendOperation; }
            MTLBlendFactor getSourceRGBBlendFactor() const { return sourceRGBBlendFactor; }
            MTLBlendFactor getDestinationRGBBlendFactor() const { return destinationRGBBlendFactor; }
            MTLBlendFactor getSourceAlphaBlendFactor() const { return sourceAlphaBlendFactor; }
            MTLBlendFactor getDestinationAlphaBlendFactor() const { return destinationAlphaBlendFactor; }

        protected:
            MTLBlendOperation rgbBlendOperation;
            MTLBlendOperation alphaBlendOperation;
            MTLBlendFactor sourceRGBBlendFactor;
            MTLBlendFactor destinationRGBBlendFactor;
            MTLBlendFactor sourceAlphaBlendFactor;
            MTLBlendFactor destinationAlphaBlendFactor;
        };
    } // namespace graphics
} // namespace ouzel
