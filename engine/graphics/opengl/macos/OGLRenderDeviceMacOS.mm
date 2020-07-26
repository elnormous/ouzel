// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "../../../core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV && OUZEL_COMPILE_OPENGL

#include "OGLRenderDeviceMacOS.hpp"
#include "OpenGLView.h"
#include "../../../core/macos/NativeWindowMacOS.hpp"
#include "../../../core/Engine.hpp"
#include "../../../utils/Log.hpp"

namespace ouzel::graphics::opengl::macos
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

    RenderDevice::RenderDevice(const std::function<void(const Event&)>& initCallback):
        opengl::RenderDevice(initCallback)
    {
        embedded = false;
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

        if (openGLContext)
        {
            [NSOpenGLContext clearCurrentContext];
            [openGLContext release];
        }

        if (pixelFormat)
            [pixelFormat release];
    }

    void RenderDevice::init(core::Window* newWindow,
                            const Size2U& newSize,
                            const Settings& settings)
    {
        constexpr NSOpenGLPixelFormatAttribute openGLVersions[] = {
            NSOpenGLProfileVersion4_1Core,
            NSOpenGLProfileVersion3_2Core,
            NSOpenGLProfileVersionLegacy
        };

        for (const auto openGLVersion : openGLVersions)
        {
            // Create pixel format
            std::vector<NSOpenGLPixelFormatAttribute> attributes = {
                NSOpenGLPFAAccelerated,
                NSOpenGLPFANoRecovery,
                NSOpenGLPFADoubleBuffer,
                NSOpenGLPFAOpenGLProfile, openGLVersion,
                NSOpenGLPFAColorSize, 24,
                NSOpenGLPFAAlphaSize, 8,
                NSOpenGLPFADepthSize, static_cast<NSOpenGLPixelFormatAttribute>(settings.depth ? 24 : 0),
                NSOpenGLPFAStencilSize, static_cast<NSOpenGLPixelFormatAttribute>(settings.stencil ? 8 : 0)
            };

            if (settings.sampleCount)
            {
                attributes.push_back(NSOpenGLPFAMultisample);
                attributes.push_back(NSOpenGLPFASampleBuffers);
                attributes.push_back(1);
                attributes.push_back(NSOpenGLPFASamples);
                attributes.push_back(settings.sampleCount);
            }

            attributes.push_back(0);

            pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes.data()];

            if (pixelFormat)
            {
                switch (openGLVersion)
                {
                    case NSOpenGLProfileVersionLegacy:
                        apiVersion = ApiVersion(2, 0);
                        logger.log(Log::Level::info) << "OpenGL 2 pixel format created";
                        break;
                    case NSOpenGLProfileVersion3_2Core:
                        apiVersion = ApiVersion(3, 2);
                        logger.log(Log::Level::info) << "OpenGL 3.2 pixel format created";
                        break;
                    case NSOpenGLProfileVersion4_1Core:
                        apiVersion = ApiVersion(4, 1);
                        logger.log(Log::Level::info) << "OpenGL 4.1 pixel format created";
                        break;
                }
                break;
            }
        }

        if (!pixelFormat)
            throw std::runtime_error("Failed to crete OpenGL pixel format");

        // Create OpenGL context
        openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
        [openGLContext makeCurrentContext];

        auto windowMacOS = static_cast<core::macos::NativeWindow*>(newWindow->getNativeWindow());
        OpenGLView* openGLView = (OpenGLView*)windowMacOS->getNativeView();

        [openGLView setOpenGLContext:openGLContext];
        [openGLContext setView:openGLView];

        const GLint swapInterval = settings.verticalSync ? 1 : 0;
        [openGLContext setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];

        opengl::RenderDevice::init(newWindow,
                                   newSize,
                                   settings);

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

    void RenderDevice::resizeFrameBuffer()
    {
        [openGLContext update];
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

    void RenderDevice::present()
    {
        [openGLContext flushBuffer];
    }

    bool RenderDevice::handleWindow(const WindowEvent& event)
    {
        if (event.type == ouzel::Event::Type::screenChange)
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
                    throw std::runtime_error("Failed to create display link");

                if (CVDisplayLinkSetOutputCallback(displayLink, macos::renderCallback, this) != kCVReturnSuccess)
                    throw std::runtime_error("Failed to set output callback for the display link");

                if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
                    throw std::runtime_error("Failed to start display link");
            });
        }

        return false;
    }

    void RenderDevice::renderCallback()
    {
        if ([NSOpenGLContext currentContext] != openGLContext)
            [openGLContext makeCurrentContext];

        if (running) process();
    }
}
#endif
