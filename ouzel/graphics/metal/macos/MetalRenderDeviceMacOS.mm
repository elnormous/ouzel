// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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

namespace ouzel
{
    namespace graphics
    {
        namespace metal
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
                        RenderDeviceMacOS* renderDevice = static_cast<RenderDeviceMacOS*>(userInfo);
                        renderDevice->renderCallback();
                    }
                    catch (const std::exception& e)
                    {
                        engine->log(Log::Level::Error) << e.what();
                        return kCVReturnError;
                    }

                    [pool release];

                    return kCVReturnSuccess;
                }
            }

            RenderDeviceMacOS::RenderDeviceMacOS(const std::function<void(const Event&)>& initCallback):
                RenderDevice(initCallback)
            {
            }

            RenderDeviceMacOS::~RenderDeviceMacOS()
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

            void RenderDeviceMacOS::init(Window* newWindow,
                                         const Size2U& newSize,
                                         uint32_t newSampleCount,
                                         SamplerFilter newTextureFilter,
                                         uint32_t newMaxAnisotropy,
                                         bool newSrgb,
                                         bool newVerticalSync,
                                         bool newDepth,
                                         bool newStencil,
                                         bool newDebugRenderer)
            {
                RenderDevice::init(newWindow,
                                   newSize,
                                   newSampleCount,
                                   newTextureFilter,
                                   newMaxAnisotropy,
                                   newSrgb,
                                   newVerticalSync,
                                   newDepth,
                                   newStencil,
                                   newDebugRenderer);

                NativeWindowMacOS* windowMacOS = static_cast<NativeWindowMacOS*>(newWindow->getNativeWindow());
                MetalView* view = (MetalView*)windowMacOS->getNativeView();

                metalLayer = (CAMetalLayer*)view.layer;
                metalLayer.device = device;
                const CGSize drawableSize = CGSizeMake(newSize.v[0], newSize.v[1]);
                metalLayer.drawableSize = drawableSize;

                colorFormat = metalLayer.pixelFormat;

                eventHandler.windowHandler = std::bind(&RenderDeviceMacOS::handleWindow, this, std::placeholders::_1);
                engine->getEventDispatcher().addEventHandler(eventHandler);

                const CGDirectDisplayID displayId = windowMacOS->getDisplayId();
                if (CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink) != kCVReturnSuccess)
                    throw std::runtime_error("Failed to create display link");

                if (CVDisplayLinkSetOutputCallback(displayLink, metal::renderCallback, this) != kCVReturnSuccess)
                    throw std::runtime_error("Failed to set output callback for the display link");

                running = true;

                if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
                    throw std::runtime_error("Failed to start display link");
            }

            std::vector<Size2U> RenderDeviceMacOS::getSupportedResolutions() const
            {
                std::vector<Size2U> result;

                CFArrayRef displayModes = CGDisplayCopyAllDisplayModes(kCGDirectMainDisplay, nullptr);
                const CFIndex displayModeCount = CFArrayGetCount(displayModes);

                for (CFIndex i = 0; i < displayModeCount; ++i)
                {
                    const CGDisplayModeRef displayMode = (const CGDisplayModeRef)CFArrayGetValueAtIndex(displayModes, i);

                    result.emplace_back(static_cast<uint32_t>(CGDisplayModeGetWidth(displayMode)),
                                        static_cast<uint32_t>(CGDisplayModeGetHeight(displayMode)));
                }

                CFRelease(displayModes);

                return result;
            }

            bool RenderDeviceMacOS::handleWindow(const WindowEvent& event)
            {
                if (event.type == ouzel::Event::Type::ScreenChange)
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

                        if (CVDisplayLinkSetOutputCallback(displayLink, metal::renderCallback, this) != kCVReturnSuccess)
                            throw std::runtime_error("Failed to set output callback for the display link");

                        running = true;

                        if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
                            throw std::runtime_error("Failed to start display link");
                    });
                }

                return false;
            }

            void RenderDeviceMacOS::renderCallback()
            {
                if (running) process();
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
