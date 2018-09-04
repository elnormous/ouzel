// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_PLATFORM_TVOS && OUZEL_COMPILE_METAL

#include "RenderDeviceMetalTVOS.hpp"
#include "MetalView.h"
#include "core/Window.hpp"
#include "core/tvos/NativeWindowTVOS.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

static void renderCallback(void* userInfo)
{
    try
    {
        ouzel::graphics::RenderDeviceMetalTVOS* renderDevice = static_cast<ouzel::graphics::RenderDeviceMetalTVOS*>(userInfo);
        renderDevice->process();
    }
    catch (const std::exception& e)
    {
        ouzel::Log(ouzel::Log::Level::ERR) << e.what();
    }
    catch (...)
    {
        ouzel::Log(ouzel::Log::Level::ERR) << "Unknown error occurred";
    }
}

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceMetalTVOS::RenderDeviceMetalTVOS():
            displayLink(renderCallback, this)
        {
        }

        RenderDeviceMetalTVOS::~RenderDeviceMetalTVOS()
        {
            displayLink.stop();
            flushCommands();
        }

        void RenderDeviceMetalTVOS::init(Window* newWindow,
                                         const Size2& newSize,
                                         uint32_t newSampleCount,
                                         Texture::Filter newTextureFilter,
                                         uint32_t newMaxAnisotropy,
                                         bool newVerticalSync,
                                         bool newDepth,
                                         bool newDebugRenderer)
        {
            RenderDeviceMetal::init(newWindow,
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
            CGSize drawableSize = CGSizeMake(newSize.width, newSize.height);
            metalLayer.drawableSize = drawableSize;

            colorFormat = metalLayer.pixelFormat;

            displayLink.start(verticalSync);
        }
    } // namespace graphics
} // namespace ouzel

#endif
