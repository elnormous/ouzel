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
            MTLBlendOperation getRGBBlendOperation() const { return rgbBlendOperation; }
            MTLBlendOperation getAlphaBlendOperation() const { return alphaBlendOperation; }
            MTLBlendFactor getSourceRGBBlendFactor() const { return sourceRGBBlendFactor; }
            MTLBlendFactor getDestinationRGBBlendFactor() const { return destinationRGBBlendFactor; }
            MTLBlendFactor getSourceAlphaBlendFactor() const { return sourceAlphaBlendFactor; }
            MTLBlendFactor getDestinationAlphaBlendFactor() const { return destinationAlphaBlendFactor; }
            bool isMetalBlendingEnabled() const { return metalBlendingEnabled; }

        protected:
            virtual bool upload() override;

            MTLBlendOperation rgbBlendOperation;
            MTLBlendOperation alphaBlendOperation;
            MTLBlendFactor sourceRGBBlendFactor;
            MTLBlendFactor destinationRGBBlendFactor;
            MTLBlendFactor sourceAlphaBlendFactor;
            MTLBlendFactor destinationAlphaBlendFactor;
            bool metalBlendingEnabled;
        };
    } // namespace graphics
} // namespace ouzel
