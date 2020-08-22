// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALBLENDSTATE_HPP
#define OUZEL_GRAPHICS_METALBLENDSTATE_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#  import <Metal/Metal.h>
#else
#  include <objc/objc.h>
using MTLBlendFactor = NSUInteger;
using MTLBlendOperation = NSUInteger;
using MTLColorWriteMask = NSUInteger;
#endif

#include "MetalRenderResource.hpp"
#include "../BlendFactor.hpp"
#include "../BlendOperation.hpp"
#include "../ColorMask.hpp"

namespace ouzel::graphics::metal
{
    class RenderDevice;

    class BlendState final: public RenderResource
    {
    public:
        BlendState(RenderDevice& initRenderDevice,
                   bool enableBlending,
                   BlendFactor colorBlendSource,
                   BlendFactor colorBlendDest,
                   BlendOperation colorOperation,
                   BlendFactor alphaBlendSource,
                   BlendFactor alphaBlendDest,
                   BlendOperation alphaOperation,
                   ColorMask colorMask);

        auto getRgbBlendOperation() const noexcept { return rgbBlendOperation; }
        auto getAlphaBlendOperation() const noexcept { return alphaBlendOperation; }
        auto getSourceRgbBlendFactor() const noexcept { return sourceRgbBlendFactor; }
        auto getDestinationRgbBlendFactor() const noexcept { return destinationRgbBlendFactor; }
        auto getSourceAlphaBlendFactor() const noexcept { return sourceAlphaBlendFactor; }
        auto getDestinationAlphaBlendFactor() const noexcept { return destinationAlphaBlendFactor; }
        auto getColorWriteMask() const noexcept { return colorWriteMask; }
        auto isBlendingEnabled() const noexcept { return blendingEnabled; }

    private:
        MTLBlendOperation rgbBlendOperation;
        MTLBlendOperation alphaBlendOperation;
        MTLBlendFactor sourceRgbBlendFactor;
        MTLBlendFactor destinationRgbBlendFactor;
        MTLBlendFactor sourceAlphaBlendFactor;
        MTLBlendFactor destinationAlphaBlendFactor;
        MTLColorWriteMask colorWriteMask;
        bool blendingEnabled = false;
    };
}

#endif

#endif // OUZEL_GRAPHICS_METALBLENDSTATE_HPP
