// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#include "RendererMetalMacOS.h"
#include "MetalView.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "core/macos/WindowMacOS.h"
#include "utils/Log.h"

static CVReturn renderCallback(CVDisplayLinkRef,
                               const CVTimeStamp*,
                               const CVTimeStamp*,
                               CVOptionFlags,
                               CVOptionFlags*,
                               void*)
{
    @autoreleasepool
    {
        if (ouzel::sharedEngine->isRunning() && !ouzel::sharedEngine->draw())
        {
            ouzel::sharedApplication->execute([] {
                ouzel::sharedEngine->getWindow()->close();
            });
        }
    }

    return kCVReturnSuccess;
}

namespace ouzel
{
    namespace graphics
    {
        RendererMetalMacOS::~RendererMetalMacOS()
        {
            if (displayLink)
            {
                if (CVDisplayLinkStop(displayLink) != kCVReturnSuccess)
                {
                    Log(Log::Level::ERR) << "Failed to stop display link";
                }

                CVDisplayLinkRelease(displayLink);
            }
        }

        bool RendererMetalMacOS::init(Window* newWindow,
                                      const Size2& newSize,
                                      uint32_t newSampleCount,
                                      Texture::Filter newTextureFilter,
                                      uint32_t newMaxAnisotropy,
                                      bool newVerticalSync,
                                      bool newDepth,
                                      bool newDebugRenderer)
        {
            if (!RendererMetal::init(newWindow,
                                     newSize,
                                     newSampleCount,
                                     newTextureFilter,
                                     newMaxAnisotropy,
                                     newVerticalSync,
                                     newDepth,
                                     newDebugRenderer))
            {
                return false;
            }

            MetalView* view = (MetalView*)static_cast<WindowMacOS*>(newWindow)->getNativeView();

            metalLayer = (CAMetalLayer*)view.layer;
            metalLayer.device = device;
            CGSize drawableSize = CGSizeMake(newSize.v[0], newSize.v[1]);
            metalLayer.drawableSize = drawableSize;

            colorFormat = metalLayer.pixelFormat;

            eventHandler.windowHandler = std::bind(&RendererMetalMacOS::handleWindow, this, std::placeholders::_1, std::placeholders::_2);
            sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

            WindowMacOS* windowMacOS = static_cast<WindowMacOS*>(newWindow);

            const CGDirectDisplayID displayId = [[[[windowMacOS->getNativeWindow() screen] deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];

            if (CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink) != kCVReturnSuccess)
            {
                Log(Log::Level::ERR) << "Failed to create display link";
                return false;
            }

            if (CVDisplayLinkSetOutputCallback(displayLink, renderCallback, nullptr) != kCVReturnSuccess)
            {
                Log(Log::Level::ERR) << "Failed to set output callback for the display link";
                return false;
            }

            if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
            {
                Log(Log::Level::ERR) << "Failed to start display link";
                return false;
            }

            return true;
        }

        bool RendererMetalMacOS::handleWindow(Event::Type type, const WindowEvent& event)
        {
            if (type == Event::Type::WINDOW_SCREEN_CHANGE)
            {
                sharedApplication->execute([this, event]() {
                    if (displayLink)
                    {
                        if (CVDisplayLinkStop(displayLink) != kCVReturnSuccess)
                        {
                            Log(Log::Level::ERR) << "Failed to stop display link";
                        }

                        CVDisplayLinkRelease(displayLink);
                        displayLink = nullptr;
                    }

                    const CGDirectDisplayID displayId = event.screenId;

                    if (CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink) != kCVReturnSuccess)
                    {
                        Log(Log::Level::ERR) << "Failed to create display link";
                        return;
                    }

                    if (CVDisplayLinkSetOutputCallback(displayLink, renderCallback, nullptr) != kCVReturnSuccess)
                    {
                        Log(Log::Level::ERR) << "Failed to set output callback for the display link";
                        return;
                    }

                    if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
                    {
                        Log(Log::Level::ERR) << "Failed to start display link";
                        return;
                    }
                });
            }
            
            return true;
        }

    } // namespace graphics
} // namespace ouzel

#endif
