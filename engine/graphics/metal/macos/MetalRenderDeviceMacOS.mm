// Ouzel by Elviss Strazdins

#include "../../../core/Setup.h"

#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV && OUZEL_COMPILE_METAL

#include <stdexcept>
#include "MetalRenderDeviceMacOS.hpp"
#include "MetalView.h"
#include "../../../core/Engine.hpp"
#include "../../../core/macos/NativeWindowMacOS.hpp"
#include "../../../platform/foundation/AutoreleasePool.hpp"
#include "../../../utils/Bit.hpp"
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
        metal::RenderDevice{settings, initWindow, initCallback},
        displayLink{static_cast<core::macos::NativeWindow*>(window.getNativeWindow())->getDisplayId()}
    {
        const auto windowMacOS = static_cast<core::macos::NativeWindow*>(window.getNativeWindow());
        MetalView* view = (MetalView*)windowMacOS->getNativeView();

        metalLayer = (CAMetalLayer*)view.layer;
        metalLayer.device = device.get();
        const CGSize drawableSize = CGSizeMake(window.getResolution().v[0],
                                               window.getResolution().v[1]);
        metalLayer.drawableSize = drawableSize;

        colorFormat = metalLayer.pixelFormat;

        eventHandler.windowHandler = std::bind(&RenderDevice::handleWindow, this, std::placeholders::_1);
        engine->getEventDispatcher().addEventHandler(eventHandler);

        displayLink.setCallback(macos::renderCallback, this);

        running = true;
    }

    RenderDevice::~RenderDevice()
    {
        running = false;
        CommandBuffer commandBuffer;
        commandBuffer.pushCommand(std::make_unique<PresentCommand>());
        submitCommandBuffer(std::move(commandBuffer));
    }

    std::vector<math::Size<std::uint32_t, 2>> RenderDevice::getSupportedResolutions() const
    {
        std::vector<math::Size<std::uint32_t, 2>> result;

        const CFArrayRef displayModes = CGDisplayCopyAllDisplayModes(kCGDirectMainDisplay, nullptr);
        const CFIndex displayModeCount = CFArrayGetCount(displayModes);

        for (CFIndex i = 0; i < displayModeCount; ++i)
        {
            const auto displayMode = bitCast<CGDisplayModeRef>(CFArrayGetValueAtIndex(displayModes, i));

            const math::Size<std::uint32_t, 2> resolution{
                static_cast<std::uint32_t>(CGDisplayModeGetWidth(displayMode)),
                static_cast<std::uint32_t>(CGDisplayModeGetHeight(displayMode))
            };

            result.emplace_back(resolution);
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

                const CGDirectDisplayID displayId = event.screenId;
                displayLink = platform::corevideo::DisplayLink{displayId};
                displayLink.setCallback(macos::renderCallback, this);

                running = true;

                displayLink.start();
            });
        }

        return false;
    }

    void RenderDevice::start()
    {
        displayLink.start();
    }

    void RenderDevice::renderCallback()
    {
        platform::foundation::AutoreleasePool autoreleasePool;
        if (running) process();
    }
}

#endif
