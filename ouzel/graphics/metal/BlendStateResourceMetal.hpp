// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#  import <Metal/Metal.h>
#else
#  include <objc/objc.h>
typedef NSUInteger MTLBlendFactor;
typedef NSUInteger MTLBlendOperation;
typedef NSUInteger MTLColorWriteMask;
#endif

#include "graphics/RenderResource.hpp"
#include "graphics/BlendState.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class BlendStateResourceMetal: public RenderResource
        {
        public:
            BlendStateResourceMetal(RenderDeviceMetal& renderDeviceMetal);

            void init(bool newEnableBlending,
                      BlendState::Factor newColorBlendSource, BlendState::Factor newColorBlendDest,
                      BlendState::Operation newColorOperation,
                      BlendState::Factor newAlphaBlendSource, BlendState::Factor newAlphaBlendDest,
                      BlendState::Operation newAlphaOperation,
                      uint8_t newColorMask);

            inline MTLBlendOperation getRGBBlendOperation() const { return rgbBlendOperation; }
            inline MTLBlendOperation getAlphaBlendOperation() const { return alphaBlendOperation; }
            inline MTLBlendFactor getSourceRGBBlendFactor() const { return sourceRGBBlendFactor; }
            inline MTLBlendFactor getDestinationRGBBlendFactor() const { return destinationRGBBlendFactor; }
            inline MTLBlendFactor getSourceAlphaBlendFactor() const { return sourceAlphaBlendFactor; }
            inline MTLBlendFactor getDestinationAlphaBlendFactor() const { return destinationAlphaBlendFactor; }
            inline MTLColorWriteMask getColorWriteMask() const { return colorWriteMask; }
            inline bool isMetalBlendingEnabled() const { return metalBlendingEnabled; }

        private:
            BlendState::Factor colorBlendSource = BlendState::Factor::ONE;
            BlendState::Factor colorBlendDest = BlendState::Factor::ZERO;
            BlendState::Operation colorOperation = BlendState::Operation::ADD;
            BlendState::Factor alphaBlendSource = BlendState::Factor::ONE;
            BlendState::Factor alphaBlendDest = BlendState::Factor::ZERO;
            BlendState::Operation alphaOperation = BlendState::Operation::ADD;
            uint8_t colorMask = BlendState::COLOR_MASK_ALL;
            bool enableBlending = false;

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
