// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALBLENDSTATE_HPP
#define OUZEL_GRAPHICS_METALBLENDSTATE_HPP

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

#include "graphics/metal/MetalRenderResource.hpp"
#include "graphics/BlendFactor.hpp"
#include "graphics/BlendOperation.hpp"
#include "graphics/ColorMask.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            class RenderDevice;

            class BlendState final: public RenderResource
            {
            public:
                BlendState(RenderDevice& renderDevice,
                           bool enableBlending,
                           BlendFactor colorBlendSource,
                           BlendFactor colorBlendDest,
                           BlendOperation colorOperation,
                           BlendFactor alphaBlendSource,
                           BlendFactor alphaBlendDest,
                           BlendOperation alphaOperation,
                           uint8_t colorMask);

                inline MTLBlendOperation getRgbBlendOperation() const { return rgbBlendOperation; }
                inline MTLBlendOperation getAlphaBlendOperation() const { return alphaBlendOperation; }
                inline MTLBlendFactor getSourceRgbBlendFactor() const { return sourceRgbBlendFactor; }
                inline MTLBlendFactor getDestinationRgbBlendFactor() const { return destinationRgbBlendFactor; }
                inline MTLBlendFactor getSourceAlphaBlendFactor() const { return sourceAlphaBlendFactor; }
                inline MTLBlendFactor getDestinationAlphaBlendFactor() const { return destinationAlphaBlendFactor; }
                inline MTLColorWriteMask getColorWriteMask() const { return colorWriteMask; }
                inline bool isBlendingEnabled() const { return blendingEnabled; }

            private:
                MTLBlendOperation rgbBlendOperation;
                MTLBlendOperation alphaBlendOperation;
                MTLBlendFactor sourceRgbBlendFactor;
                MTLBlendFactor destinationRgbBlendFactor;
                MTLBlendFactor sourceAlphaBlendFactor;
                MTLBlendFactor destinationAlphaBlendFactor;
                MTLColorWriteMask colorWriteMask;
                bool blendingEnabled;
            };
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALBLENDSTATE_HPP
