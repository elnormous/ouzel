// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererMetalIOS.h"
#include "MetalView.h"
#include "core/ios/WindowIOS.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        bool RendererMetalIOS::init(Window* newWindow,
                                    const Size2& newSize,
                                    uint32_t newSampleCount,
                                    Texture::Filter newTextureFilter,
                                    uint32_t newMaxAnisotropy,
                                    PixelFormat newBackBufferFormat,
                                    bool newVerticalSync,
                                    bool newDepth)
        {
            if (!RendererMetal::init(newWindow, newSize, newSampleCount,
                                     newTextureFilter, newMaxAnisotropy,
                                     newBackBufferFormat, newVerticalSync,
                                     newDepth))
            {
                return false;
            }

            MetalView* view = (MetalView*)static_cast<WindowIOS*>(newWindow)->getNativeView();

            metalLayer = (CAMetalLayer*)view.layer;
            metalLayer.device = device;
            metalLayer.contentsScale = newWindow->getContentScale();
            CGSize drawableSize = CGSizeMake(newSize.v[0], newSize.v[1]);
            metalLayer.drawableSize = drawableSize;

            colorFormat = metalLayer.pixelFormat;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
