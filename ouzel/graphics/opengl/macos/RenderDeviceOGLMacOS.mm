// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_PLATFORM_MACOS && OUZEL_COMPILE_OPENGL

#include "RenderDeviceOGLMacOS.hpp"
#include "core/macos/NativeWindowMacOS.hpp"
#include "core/Engine.hpp"
#include "utils/Errors.hpp"
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
            ouzel::graphics::RenderDeviceOGLMacOS* renderDevice = static_cast<ouzel::graphics::RenderDeviceOGLMacOS*>(userInfo);
            renderDevice->renderCallback();
        }
        catch (const std::exception& e)
        {
            ouzel::Log(ouzel::Log::Level::ERR) << e.what();
            return kCVReturnError;
        }
        catch (...)
        {
            ouzel::Log(ouzel::Log::Level::ERR) << "Unknown error occurred";
            return kCVReturnError;
        }
    }

    return kCVReturnSuccess;
}

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceOGLMacOS::RenderDeviceOGLMacOS():
            running(false)
        {
        }

        RenderDeviceOGLMacOS::~RenderDeviceOGLMacOS()
        {
            running = false;
            flushCommands();

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

        void RenderDeviceOGLMacOS::init(Window* newWindow,
                                        const Size2& newSize,
                                        uint32_t newSampleCount,
                                        Texture::Filter newTextureFilter,
                                        uint32_t newMaxAnisotropy,
                                        bool newVerticalSync,
                                        bool newDepth,
                                        bool newDebugRenderer)
        {
            NSOpenGLPixelFormatAttribute openGLVersions[] = {
                NSOpenGLProfileVersion4_1Core,
                NSOpenGLProfileVersion3_2Core,
                NSOpenGLProfileVersionLegacy
            };

            for (NSOpenGLPixelFormatAttribute openGLVersion : openGLVersions)
            {
                // Create pixel format
                std::vector<NSOpenGLPixelFormatAttribute> attributes =
                {
                    NSOpenGLPFAAccelerated,
                    NSOpenGLPFANoRecovery,
                    NSOpenGLPFADoubleBuffer,
                    NSOpenGLPFAOpenGLProfile, openGLVersion,
                    NSOpenGLPFAColorSize, 24,
                    NSOpenGLPFAAlphaSize, 8,
                    NSOpenGLPFADepthSize, static_cast<NSOpenGLPixelFormatAttribute>(newDepth ? 24 : 0)
                };

                if (newSampleCount)
                {
                    attributes.push_back(NSOpenGLPFAMultisample);
                    attributes.push_back(NSOpenGLPFASampleBuffers);
                    attributes.push_back(1);
                    attributes.push_back(NSOpenGLPFASamples);
                    attributes.push_back(newSampleCount);
                }

                attributes.push_back(0);

                pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes.data()];

                if (pixelFormat)
                {
                    switch (openGLVersion)
                    {
                        case NSOpenGLProfileVersionLegacy:
                            apiMajorVersion = 2;
                            apiMinorVersion = 0;
                            Log(Log::Level::INFO) << "OpenGL 2 pixel format created";
                            break;
                        case NSOpenGLProfileVersion3_2Core:
                            apiMajorVersion = 3;
                            apiMinorVersion = 2;
                            Log(Log::Level::INFO) << "OpenGL 3.2 pixel format created";
                            break;
                        case NSOpenGLProfileVersion4_1Core:
                            apiMajorVersion = 4;
                            apiMinorVersion = 1;
                            Log(Log::Level::INFO) << "OpenGL 4.1 pixel format created";
                            break;
                    }
                    break;
                }
            }

            if (!pixelFormat)
                throw SystemError("Failed to crete OpenGL pixel format");

            // Create OpenGL context
            openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
            [openGLContext makeCurrentContext];

            NativeWindowMacOS* windowMacOS = static_cast<NativeWindowMacOS*>(newWindow->getNativeWindow());

            [openGLContext setView:windowMacOS->getNativeView()];

            GLint swapInt = newVerticalSync ? 1 : 0;
            [openGLContext setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

            RenderDeviceOGL::init(newWindow,
                                  newSize,
                                  newSampleCount,
                                  newTextureFilter,
                                  newMaxAnisotropy,
                                  newVerticalSync,
                                  newDepth,
                                  newDebugRenderer);

            eventHandler.windowHandler = std::bind(&RenderDeviceOGLMacOS::handleWindow, this, std::placeholders::_1, std::placeholders::_2);
            engine->getEventDispatcher().addEventHandler(&eventHandler);

            CGDirectDisplayID displayId = windowMacOS->getDisplayId();
            if (CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink) != kCVReturnSuccess)
                throw SystemError("Failed to create display link");

            if (CVDisplayLinkSetOutputCallback(displayLink, ::renderCallback, this) != kCVReturnSuccess)
                throw SystemError("Failed to set output callback for the display link");

            running = true;

            if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
                throw SystemError("Failed to start display link");
        }

        void RenderDeviceOGLMacOS::setSize(const Size2& newSize)
        {
            RenderDeviceOGL::setSize(newSize);

            [openGLContext update];
        }

        std::vector<Size2> RenderDeviceOGLMacOS::getSupportedResolutions() const
        {
            std::vector<Size2> result;

            CFArrayRef displayModes = CGDisplayCopyAllDisplayModes(kCGDirectMainDisplay, nullptr);
            const CFIndex displayModeCount = CFArrayGetCount(displayModes);

            for (CFIndex i = 0; i < displayModeCount; ++i)
            {
                const CGDisplayModeRef displayMode = (const CGDisplayModeRef)CFArrayGetValueAtIndex(displayModes, i);

                result.push_back(Size2(static_cast<float>(CGDisplayModeGetWidth(displayMode)),
                                       static_cast<float>(CGDisplayModeGetHeight(displayMode))));
            }

            CFRelease(displayModes);

            return result;
        }

        void RenderDeviceOGLMacOS::present()
        {
            [openGLContext flushBuffer];
        }

        bool RenderDeviceOGLMacOS::handleWindow(Event::Type type, const WindowEvent& event)
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

            return false;
        }

        void RenderDeviceOGLMacOS::renderCallback()
        {
            [openGLContext makeCurrentContext];
            if (running) process();
        }
    } // namespace graphics
} // namespace ouzel

#endif
