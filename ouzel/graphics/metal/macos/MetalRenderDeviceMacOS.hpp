// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALRENDERDEVICEMACOS_HPP
#define OUZEL_GRAPHICS_METALRENDERDEVICEMACOS_HPP

#include "core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV && OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#  import <CoreVideo/CoreVideo.h>
#  import <QuartzCore/QuartzCore.h>
#  import <Metal/Metal.h>
typedef CAMetalLayer* CAMetalLayerPtr;
typedef id<MTLDevice> MTLDevicePtr;
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
typedef id<MTLSamplerState> MTLSamplerStatePtr;
typedef id<MTLCommandQueue> MTLCommandQueuePtr;
typedef id<MTLRenderPipelineState> MTLRenderPipelineStatePtr;
typedef id<MTLCommandBuffer> MTLCommandBufferPtr;
typedef id<MTLRenderCommandEncoder> MTLRenderCommandEncoderPtr;
typedef id<MTLTexture> MTLTexturePtr;
typedef id<MTLDepthStencilState> MTLDepthStencilStatePtr;
#else
#  include <objc/objc.h>
#  include <objc/NSObjCRuntime.h>
typedef void* CVDisplayLinkRef;
typedef id CAMetalLayerPtr;
typedef id MTLDevicePtr;
typedef id MTLRenderPassDescriptorPtr;
typedef id MTLSamplerStatePtr;
typedef id MTLCommandQueuePtr;
typedef id MTLRenderPipelineStatePtr;
typedef id MTLCommandBufferPtr;
typedef id MTLRenderCommandEncoderPtr;
typedef id MTLTexturePtr;
typedef id MTLDepthStencilStatePtr;
#endif

#include "graphics/metal/MetalRenderDevice.hpp"
#include "events/EventHandler.hpp"

namespace ouzel
{
    namespace graphics
    {
        class MetalRenderDeviceMacOS final: public MetalRenderDevice
        {
            friend Renderer;
        public:
            MetalRenderDeviceMacOS(const std::function<void(const Event&)>& initCallback);
            ~MetalRenderDeviceMacOS();

            std::vector<Size2<uint32_t>> getSupportedResolutions() const override;

            void renderCallback();

        private:
            void init(Window* newWindow,
                      const Size2<uint32_t>& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newStencil,
                      bool newDebugRenderer) override;

            bool handleWindow(const WindowEvent& event);

            CVDisplayLinkRef displayLink = nullptr;
            EventHandler eventHandler;

            std::atomic_bool running{false};
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALRENDERDEVICEMACOS_HPP
