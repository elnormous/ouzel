// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "graphics/BlendStateResource.hpp"

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

            virtual void init(bool newEnableBlending,
                              BlendState::Factor newColorBlendSource, BlendState::Factor newColorBlendDest,
                              BlendState::Operation newColorOperation,
                              BlendState::Factor newAlphaBlendSource, BlendState::Factor newAlphaBlendDest,
                              BlendState::Operation newAlphaOperation,
                              uint8_t newColorMask) override;

            inline MTLBlendOperation getRGBBlendOperation() const { return rgbBlendOperation; }
            inline MTLBlendOperation getAlphaBlendOperation() const { return alphaBlendOperation; }
            inline MTLBlendFactor getSourceRGBBlendFactor() const { return sourceRGBBlendFactor; }
            inline MTLBlendFactor getDestinationRGBBlendFactor() const { return destinationRGBBlendFactor; }
            inline MTLBlendFactor getSourceAlphaBlendFactor() const { return sourceAlphaBlendFactor; }
            inline MTLBlendFactor getDestinationAlphaBlendFactor() const { return destinationAlphaBlendFactor; }
            inline MTLColorWriteMask getColorWriteMask() const { return colorWriteMask; }
            inline bool isMetalBlendingEnabled() const { return metalBlendingEnabled; }

        private:
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
