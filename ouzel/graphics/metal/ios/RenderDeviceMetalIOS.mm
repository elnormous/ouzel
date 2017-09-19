// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_PLATFORM_IOS && OUZEL_COMPILE_METAL

#import "core/ios/DisplayLinkHandler.h"
#include "RenderDeviceMetalIOS.hpp"
#include "MetalView.h"
#include "core/ios/WindowIOS.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceMetalIOS::~RenderDeviceMetalIOS()
        {
            if (displayLinkHandler) [displayLinkHandler stop];
            flushCommands();
            if (displayLinkHandler) [displayLinkHandler dealloc];
        }

        bool RenderDeviceMetalIOS::init(Window* newWindow,
                                        const Size2& newSize,
                                        uint32_t newSampleCount,
                                        Texture::Filter newTextureFilter,
                                        uint32_t newMaxAnisotropy,
                                        bool newVerticalSync,
                                        bool newExclusiveFullscreen,
                                        bool newDepth,
                                        bool newDebugRenderer)
        {
            if (!RenderDeviceMetal::init(newWindow,
                                         newSize,
                                         newSampleCount,
                                         newTextureFilter,
                                         newMaxAnisotropy,
                                         newVerticalSync,
                                         newExclusiveFullscreen,
                                         newDepth,
                                         newDebugRenderer))
            {
                return false;
            }

            MetalView* view = (MetalView*)static_cast<WindowIOS*>(newWindow)->getNativeView();

            metalLayer = (CAMetalLayer*)view.layer;
            metalLayer.device = device;
            metalLayer.contentsScale = newWindow->getContentScale();
            CGSize drawableSize = CGSizeMake(newSize.width, newSize.height);
            metalLayer.drawableSize = drawableSize;

            colorFormat = metalLayer.pixelFormat;

            displayLinkHandler = [[DisplayLinkHandler alloc] initWithRenderDevice:this andVerticalSync:verticalSync];

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
