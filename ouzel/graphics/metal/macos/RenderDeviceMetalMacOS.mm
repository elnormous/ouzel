// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_PLATFORM_MACOS && OUZEL_COMPILE_METAL

#include "RenderDeviceMetalMacOS.hpp"
#include "MetalView.h"
#include "core/Engine.hpp"
#include "core/macos/WindowResourceMacOS.hpp"
#include "utils/Errors.hpp"

static CVReturn renderCallback(CVDisplayLinkRef,
                               const CVTimeStamp*,
                               const CVTimeStamp*,
                               CVOptionFlags,
                               CVOptionFlags*,
                               void* userInfo)
{
    @autoreleasepool
    {
        ouzel::graphics::RenderDeviceMetalMacOS* renderDevice = static_cast<ouzel::graphics::RenderDeviceMetalMacOS*>(userInfo);

        renderDevice->renderCallback();
    }

    return kCVReturnSuccess;
}

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceMetalMacOS::RenderDeviceMetalMacOS():
            running(false)
        {
        }

        RenderDeviceMetalMacOS::~RenderDeviceMetalMacOS()
        {
            running = false;
            flushCommands();

            if (displayLink)
            {
                CVDisplayLinkStop(displayLink);

                CVDisplayLinkRelease(displayLink);
            }
        }

        void RenderDeviceMetalMacOS::init(Window* newWindow,
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

            WindowResourceMacOS* windowMacOS = static_cast<WindowResourceMacOS*>(newWindow->getResource());

            MetalView* view = (MetalView*)windowMacOS->getNativeView();

            metalLayer = (CAMetalLayer*)view.layer;
            metalLayer.device = device;
            CGSize drawableSize = CGSizeMake(newSize.width, newSize.height);
            metalLayer.drawableSize = drawableSize;

            colorFormat = metalLayer.pixelFormat;

            eventHandler.windowHandler = std::bind(&RenderDeviceMetalMacOS::handleWindow, this, std::placeholders::_1, std::placeholders::_2);
            engine->getEventDispatcher()->addEventHandler(&eventHandler);

            CGDirectDisplayID displayId = windowMacOS->getDisplayId();
            if (CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink) != kCVReturnSuccess)
                throw SystemError("Failed to create display link");

            if (CVDisplayLinkSetOutputCallback(displayLink, ::renderCallback, this) != kCVReturnSuccess)
                throw SystemError("Failed to set output callback for the display link");

            running = true;

            if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
                throw SystemError("Failed to start display link");
        }

        std::vector<Size2> RenderDeviceMetalMacOS::getSupportedResolutions() const
        {
            std::vector<Size2> result;

            CFArrayRef displayModes = CGDisplayCopyAllDisplayModes(kCGDirectMainDisplay, nullptr);
            const CFIndex displayModeCount = CFArrayGetCount(displayModes);

            for (CFIndex i = 0; i < displayModeCount; i++)
            {
                const CGDisplayModeRef displayMode = (const CGDisplayModeRef)CFArrayGetValueAtIndex(displayModes, i);

                result.push_back(Size2(static_cast<float>(CGDisplayModeGetWidth(displayMode)),
                                       static_cast<float>(CGDisplayModeGetHeight(displayMode))));
            }

            CFRelease(displayModes);

            return result;
        }

        bool RenderDeviceMetalMacOS::handleWindow(Event::Type type, const WindowEvent& event)
        {
            if (type == Event::Type::SCREEN_CHANGE)
            {
                engine->executeOnMainThread([this, event]() {
                    if (displayLink)
                    {
                        CVDisplayLinkStop(displayLink);
                        CVDisplayLinkRelease(displayLink);
                        displayLink = nullptr;
                    }

                    const CGDirectDisplayID displayId = event.screenId;

                    if (CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink) != kCVReturnSuccess)
                        throw SystemError("Failed to create display link");

                    if (CVDisplayLinkSetOutputCallback(displayLink, ::renderCallback, this) != kCVReturnSuccess)
                        throw SystemError("Failed to set output callback for the display link");

                    if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
                        throw SystemError("Failed to start display link");
                });
            }

            return true;
        }

        void RenderDeviceMetalMacOS::renderCallback()
        {
            if (running) process();
        }
    } // namespace graphics
} // namespace ouzel

#endif
