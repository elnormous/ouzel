// Ouzel by Elviss Strazdins

#include "../../../core/Setup.h"

#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV && OUZEL_COMPILE_OPENGL

#include <array>
#include <utility>
#include "OGLRenderDeviceMacOS.hpp"
#include "OpenGLView.h"
#include "../../../core/Engine.hpp"
#include "../../../platform/foundation/AutoreleasePool.hpp"
#include "../../../core/macos/NativeWindowMacOS.hpp"
#include "../../../utils/Bit.hpp"
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
            platform::foundation::AutoreleasePool autoreleasePool;

            try
            {
                const auto renderDevice = static_cast<RenderDevice*>(userInfo);
                renderDevice->renderCallback();
            }
            catch (const std::exception& e)
            {
                logger.log(Log::Level::error) << e.what();
                return kCVReturnError;
            }

            return kCVReturnSuccess;
        }
    }

    RenderDevice::RenderDevice(const Settings& settings,
                               core::Window& initWindow,
                               const std::function<void(const Event&)>& initCallback):
        opengl::RenderDevice{settings, initWindow, initCallback},
        displayLink{static_cast<core::macos::NativeWindow*>(window.getNativeWindow())->getDisplayId()}
    {
        embedded = false;

        constexpr std::array<std::pair<NSOpenGLPixelFormatAttribute, ApiVersion>, 3> openGlVersions = {
            std::pair(NSOpenGLProfileVersion4_1Core, ApiVersion{4, 1}),
            std::pair(NSOpenGLProfileVersion3_2Core, ApiVersion{3, 2}),
            std::pair(NSOpenGLProfileVersionLegacy, ApiVersion{2, 0})
        };

        for (const auto& [openGlProfileVersion, openGlVersion] : openGlVersions)
        {
            // Create pixel format
            std::vector<NSOpenGLPixelFormatAttribute> attributes = {
                NSOpenGLPFAAccelerated,
                NSOpenGLPFANoRecovery,
                NSOpenGLPFADoubleBuffer,
                NSOpenGLPFAOpenGLProfile, openGlProfileVersion,
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
                apiVersion = openGlVersion;
                logger.log(Log::Level::info) << "OpenGL " << apiVersion << " pixel format created";
                break;
            }
        }

        if (!pixelFormat)
            throw std::runtime_error("Failed to crete OpenGL pixel format");

        // Create OpenGL context
        context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
        [context makeCurrentContext];

        const auto windowMacOS = static_cast<core::macos::NativeWindow*>(window.getNativeWindow());
        OpenGLView* openGlView = (OpenGLView*)windowMacOS->getNativeView();

        [openGlView setOpenGLContext:context];
        [context setView:openGlView];

        const GLint swapInterval = settings.verticalSync ? 1 : 0;
        [context setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];

        init(static_cast<GLsizei>(window.getResolution().v[0]),
             static_cast<GLsizei>(window.getResolution().v[1]));

        eventHandler.windowHandler = std::bind(&RenderDevice::handleWindow, this, std::placeholders::_1);
        engine->getEventDispatcher().addEventHandler(eventHandler);

        displayLink.setCallback(macos::renderCallback, this);

        running = true;

        displayLink.start();
    }

    RenderDevice::~RenderDevice()
    {
        running = false;
        CommandBuffer commandBuffer;
        commandBuffer.pushCommand(std::make_unique<PresentCommand>());
        submitCommandBuffer(std::move(commandBuffer));

        if (context)
        {
            [NSOpenGLContext clearCurrentContext];
            [context release];
        }

        if (pixelFormat)
            [pixelFormat release];
    }

    void RenderDevice::resizeFrameBuffer()
    {
        [context update];
    }

    std::vector<Size<std::uint32_t, 2>> RenderDevice::getSupportedResolutions() const
    {
        std::vector<Size<std::uint32_t, 2>> result;

        const CFArrayRef displayModes = CGDisplayCopyAllDisplayModes(kCGDirectMainDisplay, nullptr);
        const CFIndex displayModeCount = CFArrayGetCount(displayModes);

        for (CFIndex i = 0; i < displayModeCount; ++i)
        {
            const auto displayMode = bitCast<CGDisplayModeRef>(CFArrayGetValueAtIndex(displayModes, i));

            result.emplace_back(static_cast<std::uint32_t>(CGDisplayModeGetWidth(displayMode)),
                                static_cast<std::uint32_t>(CGDisplayModeGetHeight(displayMode)));
        }

        CFRelease(displayModes);

        return result;
    }

    void RenderDevice::present()
    {
        [context flushBuffer];
    }

    bool RenderDevice::handleWindow(const WindowEvent& event)
    {
        if (event.type == ouzel::Event::Type::screenChange)
        {
            engine->executeOnMainThread([this, event]() {
                const CGDirectDisplayID displayId = event.screenId;
                displayLink = platform::corevideo::DisplayLink{displayId};
                displayLink.setCallback(macos::renderCallback, this);

                displayLink.start();
            });
        }

        return false;
    }

    void RenderDevice::renderCallback()
    {
        if ([NSOpenGLContext currentContext] != context)
            [context makeCurrentContext];

        if (running) process();
    }
}
#endif
