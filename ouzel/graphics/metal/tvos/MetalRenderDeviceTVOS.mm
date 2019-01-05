// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if TARGET_OS_TV && OUZEL_COMPILE_METAL

#include "MetalRenderDeviceTVOS.hpp"
#include "MetalView.h"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "core/tvos/NativeWindowTVOS.hpp"
#include "utils/Log.hpp"

static void renderCallback(void* userInfo)
{
    try
    {
        ouzel::graphics::MetalRenderDeviceTVOS* renderDevice = static_cast<ouzel::graphics::MetalRenderDeviceTVOS*>(userInfo);
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
        MetalRenderDeviceTVOS::MetalRenderDeviceTVOS(const std::function<void(const Event&)>& initCallback):
            MetalRenderDevice(initCallback),
            displayLink(::renderCallback, this)
        {
        }

        MetalRenderDeviceTVOS::~MetalRenderDeviceTVOS()
        {
            displayLink.stop();
            CommandBuffer commandBuffer;
            commandBuffer.commands.push(std::unique_ptr<Command>(new PresentCommand()));
            submitCommandBuffer(std::move(commandBuffer));
        }

        void MetalRenderDeviceTVOS::init(Window* newWindow,
                                         const Size2<float>& newSize,
                                         uint32_t newSampleCount,
                                         Texture::Filter newTextureFilter,
                                         uint32_t newMaxAnisotropy,
                                         bool newVerticalSync,
                                         bool newDepth,
                                         bool newDebugRenderer)
        {
            MetalRenderDevice::init(newWindow,
                                    newSize,
                                    newSampleCount,
                                    newTextureFilter,
                                    newMaxAnisotropy,
                                    newVerticalSync,
                                    newDepth,
                                    newDebugRenderer);

            MetalView* view = (MetalView*)static_cast<NativeWindowTVOS*>(newWindow->getNativeWindow())->getNativeView();

            metalLayer = (CAMetalLayer*)view.layer;
            metalLayer.device = device;
            CGSize drawableSize = CGSizeMake(newSize.v[0], newSize.v[1]);
            metalLayer.drawableSize = drawableSize;

            colorFormat = metalLayer.pixelFormat;

            displayLink.start(verticalSync);
        }

        void MetalRenderDeviceTVOS::renderCallback()
        {
            process();
        }
    } // namespace graphics
} // namespace ouzel

#endif
