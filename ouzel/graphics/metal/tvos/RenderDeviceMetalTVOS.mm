// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_PLATFORM_TVOS && OUZEL_COMPILE_METAL

#include "RenderDeviceMetalTVOS.hpp"
#include "MetalView.h"
#include "core/Window.hpp"
#include "core/tvos/NativeWindowTVOS.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceMetalTVOS::RenderDeviceMetalTVOS():
            displayLink(*this)
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
