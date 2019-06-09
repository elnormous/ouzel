// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALRENDERTARGET_HPP
#define OUZEL_GRAPHICS_METALRENDERTARGET_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#  import <Metal/Metal.h>
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
#else
#  include <objc/objc.h>
typedef id MTLRenderPassDescriptorPtr;
typedef NSUInteger MTLLoadAction;
typedef struct
{
    double red;
    double green;
    double blue;
    double alpha;
} MTLClearColor;
#endif

#include <set>
#include "graphics/metal/MetalRenderResource.hpp"
#include "math/Color.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            class RenderDevice;
            class Texture;

            class RenderTarget final: public RenderResource
            {
            public:
                RenderTarget(RenderDevice& renderDevice,
                             const std::set<Texture*>& initColorTextures,
                             Texture* initDepthTexture);
                ~RenderTarget();

                inline const std::set<Texture*>& getColorTextures() const { return colorTextures; }
                inline Texture* getDepthTexture() const { return depthTexture; }

                inline NSUInteger getSampleCount() const { return sampleCount; }
                inline const std::vector<MTLPixelFormat>& getColorFormats() const { return colorFormats; }
                inline MTLPixelFormat getDepthFormat() const { return depthFormat; }
                inline MTLPixelFormat getStencilFormat() const { return stencilFormat; }

                inline MTLRenderPassDescriptorPtr getRenderPassDescriptor() const { return renderPassDescriptor; }

            private:
                std::set<Texture*> colorTextures;
                Texture* depthTexture = nullptr;

                NSUInteger sampleCount = 0;
                std::vector<MTLPixelFormat> colorFormats;
                MTLPixelFormat depthFormat;
                MTLPixelFormat stencilFormat;

                MTLRenderPassDescriptorPtr renderPassDescriptor = nil;
            };
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALRENDERTARGET_HPP
