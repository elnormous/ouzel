// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
                                       bool initClearColorBuffer = true,
                                       bool initClearDepthBuffer = false,
                                       Color initClearColor = Color(),
                                       float initClearDepth = 1.0F);
            ~MetalRenderTarget();

            void addColorTexture(MetalTexture* texture);
            void removeColorTexture(MetalTexture* texture);
            void setDepthTexture(MetalTexture* texture);

            NSUInteger getSampleCount() const { return sampleCount; }
            const std::vector<MTLPixelFormat>& getColorFormats() const { return colorFormats; }
            MTLPixelFormat getDepthFormat() const { return depthFormat; }

            void setClearColorBuffer(bool clear);
            void setClearDepthBuffer(bool clear);
            void setClearColor(Color color);
            void setClearDepth(float newClearDepth);

            inline MTLRenderPassDescriptorPtr getRenderPassDescriptor() const { return renderPassDescriptor; }

            inline MTLLoadAction getColorBufferLoadAction() const { return colorBufferLoadAction; }
            inline MTLLoadAction getDepthBufferLoadAction() const { return depthBufferLoadAction; }

        private:
            std::set<MetalTexture*> colorTextures;
            MetalTexture* depthTexture = nullptr;

            NSUInteger sampleCount = 0;
            std::vector<MTLPixelFormat> colorFormats;
            MTLPixelFormat depthFormat;

            MTLRenderPassDescriptorPtr renderPassDescriptor = nil;

            MTLLoadAction colorBufferLoadAction;
            MTLLoadAction depthBufferLoadAction;
            MTLClearColor clearColor{0.0, 0.0, 0.0, 0.0};
            double clearDepth = 1.0;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALRENDERTARGET_HPP
