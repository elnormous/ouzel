// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_PLATFORM_IOS && OUZEL_COMPILE_METAL

#include "RenderDeviceMetalIOS.hpp"
#include "MetalView.h"
#include "core/Window.hpp"
#include "core/ios/WindowResourceIOS.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceMetalIOS::RenderDeviceMetalIOS():
            displayLink(*this)
        {
        }

        RenderDeviceMetalIOS::~RenderDeviceMetalIOS()
        {
            displayLink.stop();
            flushCommands();
        }

        void RenderDeviceMetalIOS::init(Window* newWindow,
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

            MetalView* view = (MetalView*)static_cast<WindowResourceIOS*>(newWindow->getResource())->getNativeView();

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
