// Ouzel by Elviss Strazdins

#include "../../../core/Setup.h"

#if TARGET_OS_TV && OUZEL_COMPILE_METAL

#include "MetalRenderDeviceTVOS.hpp"
#include "MetalView.h"
#include "../../../core/Engine.hpp"
#include "../../../core/Window.hpp"
#include "../../../core/tvos/NativeWindowTVOS.hpp"
#include "../../../platform/foundation/AutoreleasePool.hpp"
#include "../../../utils/Log.hpp"

namespace ouzel::graphics::metal::tvos
{
    namespace
    {
        void renderCallback(void* userInfo)
        {
            try
            {
                const auto renderDevice = static_cast<RenderDevice*>(userInfo);
                renderDevice->renderCallback();
            }
            catch (const std::exception& e)
            {
                logger.log(Log::Level::error) << e.what();
            }
        }
    }

    RenderDevice::RenderDevice(const Settings& settings,
                               core::Window& initWindow,
                               const std::function<void(const Event&)>& initCallback):
        metal::RenderDevice{settings, initWindow, initCallback},
        displayLink{tvos::renderCallback, this}
    {
        const auto windowTVOS = static_cast<core::tvos::NativeWindow*>(window.getNativeWindow());
        MetalView* view = (MetalView*)windowTVOS->getNativeView();

        metalLayer = (CAMetalLayer*)view.layer;
        metalLayer.device = device.get();
        const CGSize drawableSize = CGSizeMake(window.getResolution().v[0],
                                               window.getResolution().v[1]);
        metalLayer.drawableSize = drawableSize;

        colorFormat = metalLayer.pixelFormat;
    }

    RenderDevice::~RenderDevice()
    {
        running = false;
        runLoop.stop();
        CommandBuffer commandBuffer;
        commandBuffer.pushCommand(std::make_unique<PresentCommand>());
        submitCommandBuffer(std::move(commandBuffer));
    }

    void RenderDevice::start()
    {
        renderThread = thread::Thread{&RenderDevice::renderMain, this};
        std::unique_lock lock{runLoopMutex};
        while (!running) runLoopCondition.wait(lock);
    }

    void RenderDevice::renderMain()
    {
        thread::setCurrentThreadName("Render");

        if (verticalSync)
        {
            std::unique_lock lock{runLoopMutex};
            runLoop = platform::foundation::RunLoop{};
            displayLink.addToRunLoop(runLoop);
            running = true;
            lock.unlock();

            runLoopCondition.notify_all();

            runLoop.run();
        }
        else
        {
            std::unique_lock lock{runLoopMutex};
            running = true;
            lock.unlock();

            runLoopCondition.notify_all();

            while (running) renderCallback();
        }
    }
    
    void RenderDevice::renderCallback()
    {
        platform::foundation::AutoreleasePool autoreleasePool;
        process();
    }
}

#endif
