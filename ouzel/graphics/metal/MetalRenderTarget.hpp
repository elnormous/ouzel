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
#endif

#include "graphics/metal/MetalRenderResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class MetalRenderDevice;

        class MetalRenderTarget final: public MetalRenderResource
        {
        public:
            explicit MetalRenderTarget(MetalRenderDevice& renderDeviceMetal);
            ~MetalRenderTarget();

            inline MTLRenderPassDescriptorPtr getRenderPassDescriptor() const { return renderPassDescriptor; }

            inline MTLLoadAction getColorBufferLoadAction() const { return colorBufferLoadAction; }
            inline MTLLoadAction getDepthBufferLoadAction() const { return depthBufferLoadAction; }

        private:
            MTLRenderPassDescriptorPtr renderPassDescriptor = nil;

            MTLLoadAction colorBufferLoadAction;
            MTLLoadAction depthBufferLoadAction;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALRENDERTARGET_HPP
