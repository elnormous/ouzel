// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_PLATFORM_TVOS && OUZEL_COMPILE_METAL

#include "RenderDeviceMetalTVOS.hpp"
#include "MetalView.h"
#include "core/Window.hpp"
#include "core/tvos/WindowResourceTVOS.hpp"
#include "utils/Log.hpp"

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

        bool RenderDeviceMetalTVOS::init(Window* newWindow,
                                         const Size2& newSize,
                                         uint32_t newSampleCount,
                                         Texture::Filter newTextureFilter,
                                         uint32_t newMaxAnisotropy,
                                         bool newVerticalSync,
                                         bool newDepth,
                                         bool newDebugRenderer)
        {
            if (!RenderDeviceMetal::init(newWindow,
                                         newSize,
                                         newSampleCount,
                                         newTextureFilter,
                                         newMaxAnisotropy,
                                         newVerticalSync,
                                         newDepth,
                                         newDebugRenderer))
                return false;

            MetalView* view = (MetalView*)static_cast<WindowResourceTVOS*>(newWindow->getResource())->getNativeView();

            metalLayer = (CAMetalLayer*)view.layer;
            metalLayer.device = device;
            CGSize drawableSize = CGSizeMake(newSize.width, newSize.height);
            metalLayer.drawableSize = drawableSize;

            colorFormat = metalLayer.pixelFormat;

            displayLink.start(verticalSync);

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
