// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "../../../core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV && OUZEL_COMPILE_METAL

#include <stdexcept>
#include "MetalRenderDeviceMacOS.hpp"
#include "MetalView.h"
#include "../../../core/Engine.hpp"
#include "../../../core/macos/NativeWindowMacOS.hpp"
#include "../../../utils/Log.hpp"

namespace ouzel::graphics::metal::macos
{
    namespace
    {
        CVReturn renderCallback(CVDisplayLinkRef,
                                const CVTimeStamp*,
                                const CVTimeStamp*,
                                CVOptionFlags,
                                CVOptionFlags*,
                                void* userInfo)
        {
            NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

            try
            {
                auto renderDevice = static_cast<RenderDevice*>(userInfo);
                renderDevice->renderCallback();
            }
            catch (const std::exception& e)
            {
                logger.log(Log::Level::error) << e.what();
                return kCVReturnError;
            }

            [pool release];

            return kCVReturnSuccess;
        }
    }

    RenderDevice::RenderDevice(const Settings& settings,
                               core::Window& initWindow,
                               const std::function<void(const Event&)>& initCallback):
        metal::RenderDevice(settings, initWindow, initCallback)
    {
    }

    RenderDevice::~RenderDevice()
    {
        running = false;
        CommandBuffer commandBuffer;
        commandBuffer.pushCommand(std::make_unique<PresentCommand>());
        submitCommandBuffer(std::move(commandBuffer));

        if (displayLink)
        {
            CVDisplayLinkStop(displayLink);
            CVDisplayLinkRelease(displayLink);
        }
    }

    void RenderDevice::init(const Settings& settings)
    {
        metal::RenderDevice::init(settings);

        auto windowMacOS = static_cast<core::macos::NativeWindow*>(window.getNativeWindow());
        MetalView* view = (MetalView*)windowMacOS->getNativeView();

        metalLayer = (CAMetalLayer*)view.layer;
        metalLayer.device = device.get();
        const CGSize drawableSize = CGSizeMake(window.getResolution().v[0],
                                               window.getResolution().v[1]);
        metalLayer.drawableSize = drawableSize;

        colorFormat = metalLayer.pixelFormat;

        eventHandler.windowHandler = std::bind(&RenderDevice::handleWindow, this, std::placeholders::_1);
        engine->getEventDispatcher().addEventHandler(eventHandler);

        const CGDirectDisplayID displayId = windowMacOS->getDisplayId();
        if (CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink) != kCVReturnSuccess)
            throw std::runtime_error("Failed to create display link");

        if (CVDisplayLinkSetOutputCallback(displayLink, macos::renderCallback, this) != kCVReturnSuccess)
            throw std::runtime_error("Failed to set output callback for the display link");

        running = true;

        if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
            throw std::runtime_error("Failed to start display link");
    }

    std::vector<Size2U> RenderDevice::getSupportedResolutions() const
    {
        std::vector<Size2U> result;

        CFArrayRef displayModes = CGDisplayCopyAllDisplayModes(kCGDirectMainDisplay, nullptr);
        const CFIndex displayModeCount = CFArrayGetCount(displayModes);

        for (CFIndex i = 0; i < displayModeCount; ++i)
        {
            const CGDisplayModeRef displayMode = (const CGDisplayModeRef)CFArrayGetValueAtIndex(displayModes, i);

            result.emplace_back(static_cast<std::uint32_t>(CGDisplayModeGetWidth(displayMode)),
                                static_cast<std::uint32_t>(CGDisplayModeGetHeight(displayMode)));
        }

        CFRelease(displayModes);

        return result;
    }

    bool RenderDevice::handleWindow(const WindowEvent& event)
    {
        if (event.type == ouzel::Event::Type::screenChange)
        {
            engine->executeOnMainThread([this, event]() {
                running = false;

                CommandBuffer commandBuffer;
                commandBuffer.pushCommand(std::make_unique<PresentCommand>());
                submitCommandBuffer(std::move(commandBuffer));

                if (displayLink)
                {
                    CVDisplayLinkStop(displayLink);
                    CVDisplayLinkRelease(displayLink);
                    displayLink = nullptr;
                }

                const CGDirectDisplayID displayId = event.screenId;

                if (CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink) != kCVReturnSuccess)
                    throw std::runtime_error("Failed to create display link");

                if (CVDisplayLinkSetOutputCallback(displayLink, macos::renderCallback, this) != kCVReturnSuccess)
                    throw std::runtime_error("Failed to set output callback for the display link");

                running = true;

                if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
                    throw std::runtime_error("Failed to start display link");
            });
        }

        return false;
    }

    void RenderDevice::renderCallback()
    {
        if (running) process();
    }
}

#endif
