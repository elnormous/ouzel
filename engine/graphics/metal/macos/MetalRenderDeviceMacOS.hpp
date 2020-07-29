// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALRENDERDEVICEMACOS_HPP
#define OUZEL_GRAPHICS_METALRENDERDEVICEMACOS_HPP

#include "../../../core/Setup.h"

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

#include "../MetalRenderDevice.hpp"
#include "../../../events/EventHandler.hpp"

namespace ouzel::graphics::metal::macos
{
    class RenderDevice final: public metal::RenderDevice
    {
        friend Graphics;
    public:
        explicit RenderDevice(const std::function<void(const Event&)>& initCallback);
        ~RenderDevice() override;

        std::vector<Size2U> getSupportedResolutions() const final;

        void renderCallback();

    private:
        void init(core::Window& newWindow,
                  const Size2U& newSize,
                  const Settings& settings) final;

        bool handleWindow(const WindowEvent& event);

        CVDisplayLinkRef displayLink = nullptr;
        EventHandler eventHandler;

        std::atomic_bool running{false};
    };
}

#endif

#endif // OUZEL_GRAPHICS_METALRENDERDEVICEMACOS_HPP
