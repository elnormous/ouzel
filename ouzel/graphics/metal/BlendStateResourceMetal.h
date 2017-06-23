// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#include "graphics/BlendStateResource.h"

#if defined(__OBJC__)
#import <Metal/Metal.h>
#else
#include <objc/objc.h>
typedef NSUInteger MTLBlendFactor;
typedef NSUInteger MTLBlendOperation;
typedef NSUInteger MTLColorWriteMask;
#endif

namespace ouzel
{
    namespace graphics
    {
        class BlendStateResourceMetal: public BlendStateResource
        {
        public:
            BlendStateResourceMetal();

            MTLBlendOperation getRGBBlendOperation() const { return rgbBlendOperation; }
            MTLBlendOperation getAlphaBlendOperation() const { return alphaBlendOperation; }
            MTLBlendFactor getSourceRGBBlendFactor() const { return sourceRGBBlendFactor; }
            MTLBlendFactor getDestinationRGBBlendFactor() const { return destinationRGBBlendFactor; }
            MTLBlendFactor getSourceAlphaBlendFactor() const { return sourceAlphaBlendFactor; }
            MTLBlendFactor getDestinationAlphaBlendFactor() const { return destinationAlphaBlendFactor; }
            MTLColorWriteMask getColorWriteMask() const { return colorWriteMask; }
            bool isMetalBlendingEnabled() const { return metalBlendingEnabled; }

        protected:
            virtual bool upload() override;

            MTLBlendOperation rgbBlendOperation;
            MTLBlendOperation alphaBlendOperation;
            MTLBlendFactor sourceRGBBlendFactor;
            MTLBlendFactor destinationRGBBlendFactor;
            MTLBlendFactor sourceAlphaBlendFactor;
            MTLBlendFactor destinationAlphaBlendFactor;
            MTLColorWriteMask colorWriteMask;
            bool metalBlendingEnabled;
        };
    } // namespace graphics
} // namespace ouzel

#endif
