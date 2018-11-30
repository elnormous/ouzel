// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_BLENDSTATEMETAL_HPP
#define OUZEL_GRAPHICS_BLENDSTATEMETAL_HPP

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

#include "graphics/metal/RenderResourceMetal.hpp"
#include "graphics/BlendState.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class BlendStateMetal final: public RenderResourceMetal
        {
        public:
            BlendStateMetal(RenderDeviceMetal& renderDeviceMetal,
                            bool enableBlending,
                            BlendState::Factor colorBlendSource, BlendState::Factor colorBlendDest,
                            BlendState::Operation colorOperation,
                            BlendState::Factor alphaBlendSource, BlendState::Factor alphaBlendDest,
                            BlendState::Operation alphaOperation,
                            uint8_t colorMask);

            inline MTLBlendOperation getRGBBlendOperation() const { return rgbBlendOperation; }
            inline MTLBlendOperation getAlphaBlendOperation() const { return alphaBlendOperation; }
            inline MTLBlendFactor getSourceRGBBlendFactor() const { return sourceRGBBlendFactor; }
            inline MTLBlendFactor getDestinationRGBBlendFactor() const { return destinationRGBBlendFactor; }
            inline MTLBlendFactor getSourceAlphaBlendFactor() const { return sourceAlphaBlendFactor; }
            inline MTLBlendFactor getDestinationAlphaBlendFactor() const { return destinationAlphaBlendFactor; }
            inline MTLColorWriteMask getColorWriteMask() const { return colorWriteMask; }
            inline bool isBlendingEnabled() const { return blendingEnabled; }

        private:
            MTLBlendOperation rgbBlendOperation;
            MTLBlendOperation alphaBlendOperation;
            MTLBlendFactor sourceRGBBlendFactor;
            MTLBlendFactor destinationRGBBlendFactor;
            MTLBlendFactor sourceAlphaBlendFactor;
            MTLBlendFactor destinationAlphaBlendFactor;
            MTLColorWriteMask colorWriteMask;
            bool blendingEnabled;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_BLENDSTATEMETAL_HPP
