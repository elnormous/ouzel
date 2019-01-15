// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV && OUZEL_COMPILE_METAL

#include <stdexcept>
#include "MetalRenderDeviceMacOS.hpp"
#include "MetalView.h"
#include "core/Engine.hpp"
#include "core/macos/NativeWindowMacOS.hpp"
#include "utils/Log.hpp"

static CVReturn renderCallback(CVDisplayLinkRef,
                               const CVTimeStamp*,
                               const CVTimeStamp*,
                               CVOptionFlags,
                               CVOptionFlags*,
                               void* userInfo)
{
    @autoreleasepool
    {
        try
        {
            ouzel::graphics::MetalRenderDeviceMacOS* renderDevice = static_cast<ouzel::graphics::MetalRenderDeviceMacOS*>(userInfo);
            renderDevice->renderCallback();
        }
        catch (const std::exception& e)
        {
            ouzel::engine->log(ouzel::Log::Level::ERR) << e.what();
            return kCVReturnError;
        }
    }

    return kCVReturnSuccess;
}

namespace ouzel
{
    namespace graphics
    {
        MetalRenderDeviceMacOS::MetalRenderDeviceMacOS(const std::function<void(const Event&)>& initCallback):
            MetalRenderDevice(initCallback)
        {
        }

        MetalRenderDeviceMacOS::~MetalRenderDeviceMacOS()
        {
            running = false;
            CommandBuffer commandBuffer;
            commandBuffer.pushCommand(std::unique_ptr<Command>(new PresentCommand()));
            submitCommandBuffer(std::move(commandBuffer));

            if (displayLink)
            {
                CVDisplayLinkStop(displayLink);
                CVDisplayLinkRelease(displayLink);
            }
        }

        void MetalRenderDeviceMacOS::init(Window* newWindow,
                                          const Size2<uint32_t>& newSize,
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

            NativeWindowMacOS* windowMacOS = static_cast<NativeWindowMacOS*>(newWindow->getNativeWindow());

            MetalView* view = (MetalView*)windowMacOS->getNativeView();

            metalLayer = (CAMetalLayer*)view.layer;
            metalLayer.device = device;
            CGSize drawableSize = CGSizeMake(newSize.v[0], newSize.v[1]);
            metalLayer.drawableSize = drawableSize;

            colorFormat = metalLayer.pixelFormat;

            eventHandler.windowHandler = std::bind(&MetalRenderDeviceMacOS::handleWindow, this, std::placeholders::_1);
            engine->getEventDispatcher().addEventHandler(&eventHandler);

            CGDirectDisplayID displayId = windowMacOS->getDisplayId();
            if (CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink) != kCVReturnSuccess)
                throw std::runtime_error("Failed to create display link");

            if (CVDisplayLinkSetOutputCallback(displayLink, ::renderCallback, this) != kCVReturnSuccess)
                throw std::runtime_error("Failed to set output callback for the display link");

            running = true;

            if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
                throw std::runtime_error("Failed to start display link");
        }

        std::vector<Size2<uint32_t>> MetalRenderDeviceMacOS::getSupportedResolutions() const
        {
            std::vector<Size2<uint32_t>> result;

            CFArrayRef displayModes = CGDisplayCopyAllDisplayModes(kCGDirectMainDisplay, nullptr);
            const CFIndex displayModeCount = CFArrayGetCount(displayModes);

            for (CFIndex i = 0; i < displayModeCount; ++i)
            {
                const CGDisplayModeRef displayMode = (const CGDisplayModeRef)CFArrayGetValueAtIndex(displayModes, i);

                result.push_back(Size2<uint32_t>(static_cast<uint32_t>(CGDisplayModeGetWidth(displayMode)),
                                                 static_cast<uint32_t>(CGDisplayModeGetHeight(displayMode))));
            }

            CFRelease(displayModes);

            return result;
        }

        bool MetalRenderDeviceMacOS::handleWindow(const WindowEvent& event)
        {
            if (event.type == ouzel::Event::Type::SCREEN_CHANGE)
            {
                engine->executeOnMainThread([this, event]() {

                    running = false;

                    CommandBuffer commandBuffer;
                    commandBuffer.pushCommand(std::unique_ptr<Command>(new PresentCommand()));
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

                    if (CVDisplayLinkSetOutputCallback(displayLink, ::renderCallback, this) != kCVReturnSuccess)
                        throw std::runtime_error("Failed to set output callback for the display link");

                    running = true;

                    if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
                        throw std::runtime_error("Failed to start display link");
                });
            }

            return false;
        }

        void MetalRenderDeviceMacOS::renderCallback()
        {
            if (running) process();
        }
    } // namespace graphics
} // namespace ouzel

#endif
