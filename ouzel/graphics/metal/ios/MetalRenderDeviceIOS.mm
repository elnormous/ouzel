// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_IOS && OUZEL_COMPILE_METAL

#include "MetalRenderDeviceIOS.hpp"
#include "MetalView.h"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "core/ios/NativeWindowIOS.hpp"
#include "utils/Log.hpp"

static void renderCallback(void* userInfo)
{
    try
    {
        ouzel::graphics::metal::RenderDeviceIOS* renderDevice = static_cast<ouzel::graphics::metal::RenderDeviceIOS*>(userInfo);
        renderDevice->renderCallback();
    }
    catch (const std::exception& e)
    {
        ouzel::engine->log(ouzel::Log::Level::ERR) << e.what();
    }
}

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            RenderDeviceIOS::RenderDeviceIOS(const std::function<void(const Event&)>& initCallback):
                RenderDevice(initCallback),
                displayLink(::renderCallback, this)
            {
            }

            RenderDeviceIOS::~RenderDeviceIOS()
            {
                displayLink.stop();
                CommandBuffer commandBuffer;
                commandBuffer.pushCommand(std::unique_ptr<Command>(new PresentCommand()));
                submitCommandBuffer(std::move(commandBuffer));
            }

            void RenderDeviceIOS::init(Window* newWindow,
                                       const Size2U& newSize,
                                       uint32_t newSampleCount,
                                       ouzel::graphics::Texture::Filter newTextureFilter,
                                       uint32_t newMaxAnisotropy,
                                       bool newVerticalSync,
                                       bool newDepth,
                                       bool newStencil,
                                       bool newDebugRenderer)
            {
                RenderDevice::init(newWindow,
                                   newSize,
                                   newSampleCount,
                                   newTextureFilter,
                                   newMaxAnisotropy,
                                   newVerticalSync,
                                   newDepth,
                                   newStencil,
                                   newDebugRenderer);

                MetalView* view = (MetalView*)static_cast<NativeWindowIOS*>(newWindow->getNativeWindow())->getNativeView();

                metalLayer = (CAMetalLayer*)view.layer;
                metalLayer.device = device;
                CGSize drawableSize = CGSizeMake(newSize.v[0], newSize.v[1]);
                metalLayer.drawableSize = drawableSize;

                colorFormat = metalLayer.pixelFormat;

                displayLink.start(verticalSync);
            }

            void RenderDeviceIOS::renderCallback()
            {
                process();
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
