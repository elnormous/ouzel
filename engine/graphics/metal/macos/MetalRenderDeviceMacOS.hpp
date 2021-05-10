// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_METALRENDERDEVICEMACOS_HPP
#define OUZEL_GRAPHICS_METALRENDERDEVICEMACOS_HPP

#include "../../../core/Setup.h"

#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV && OUZEL_COMPILE_METAL

#ifdef __OBJC__
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
#include "../../../platform/corevideo/DisplayLink.hpp"

namespace ouzel::graphics::metal::macos
{
    class RenderDevice final: public metal::RenderDevice
    {
        friend Graphics;
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow,
                     const std::function<void(const Event&)>& initCallback);
        ~RenderDevice() override;

        std::vector<Size<std::uint32_t, 2>> getSupportedResolutions() const final;

        void renderCallback();

    private:
        bool handleWindow(const WindowEvent& event);

        platform::corevideo::DisplayLink displayLink;
        EventHandler eventHandler;

        std::atomic_bool running{false};
    };
}

#endif

#endif // OUZEL_GRAPHICS_METALRENDERDEVICEMACOS_HPP
