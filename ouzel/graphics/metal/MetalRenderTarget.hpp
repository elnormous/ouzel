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
        class MetalRenderDevice;
        class MetalTexture;

        class MetalRenderTarget final: public MetalRenderResource
        {
        public:
            explicit MetalRenderTarget(MetalRenderDevice& renderDeviceMetal,
                                       const std::set<MetalTexture*>& initColorTextures,
                                       MetalTexture* initDepthTexture);
            ~MetalRenderTarget();

            const std::set<MetalTexture*>& getColorTextures() const { return colorTextures; }
            MetalTexture* getDepthTexture() const { return depthTexture; }

            NSUInteger getSampleCount() const { return sampleCount; }
            const std::vector<MTLPixelFormat>& getColorFormats() const { return colorFormats; }
            MTLPixelFormat getDepthFormat() const { return depthFormat; }
            MTLPixelFormat getStencilFormat() const { return stencilFormat; }

            inline MTLRenderPassDescriptorPtr getRenderPassDescriptor() const { return renderPassDescriptor; }

        private:
            std::set<MetalTexture*> colorTextures;
            MetalTexture* depthTexture = nullptr;

            NSUInteger sampleCount = 0;
            std::vector<MTLPixelFormat> colorFormats;
            MTLPixelFormat depthFormat;
            MTLPixelFormat stencilFormat;

            MTLRenderPassDescriptorPtr renderPassDescriptor = nil;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALRENDERTARGET_HPP
