// Ouzel by Elviss Strazdins

#include <algorithm>
#include "RenderDevice.hpp"

namespace ouzel::graphics
{
    RenderDevice::RenderDevice(Driver initDriver,
                               const Settings& settings,
                               core::Window& initWindow):
        driver{initDriver},
        window{initWindow},
        sampleCount{settings.sampleCount},
        verticalSync{settings.verticalSync},
        srgb{settings.srgb},
        depth{settings.depth},
        stencil{settings.stencil},
        debugRenderer{settings.debugRenderer},
        npotTexturesSupported{false},
        anisotropicFilteringSupported{false},
        renderTargetsSupported{false},
        clampToBorderSupported{false},
        multisamplingSupported{false},
        uintIndicesSupported{false},
        previousFrameTime{std::chrono::steady_clock::now()}
    {
    }

    void RenderDevice::process()
    {
        const auto currentTime = std::chrono::steady_clock::now();
        const auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - previousFrameTime);
        previousFrameTime = currentTime;

        const float delta = diff.count() / 1000000000.0F;

        if (delta > 0.0F)
            currentFPS = 1.0F / delta;

        accumulatedTime += delta;
        currentAccumulatedFPS += 1.0F;

        if (accumulatedTime > 1.0F)
        {
            accumulatedFPS = currentAccumulatedFPS;
            accumulatedTime = 0.0F;
            currentAccumulatedFPS = 0.0F;
        }

        std::unique_lock lock{eventQueueMutex};
        Event event;
        event.type = Event::Type::frame;
        eventQueue.push(event);
        lock.unlock();
        eventQueueCondition.notify_all();
    }

    bool RenderDevice::hasEvents() const
    {
        std::scoped_lock lock{eventQueueMutex};
        return !eventQueue.empty();
    }

    RenderDevice::Event RenderDevice::getNextEvent()
    {
        std::unique_lock lock{eventQueueMutex};
        while (eventQueue.empty()) eventQueueCondition.wait(lock);
        Event result = std::move(eventQueue.front());
        eventQueue.pop();
        return result;
    }

    std::vector<math::Size<std::uint32_t, 2>> RenderDevice::getSupportedResolutions() const
    {
        return std::vector<math::Size<std::uint32_t, 2>>{};
    }

    void RenderDevice::generateScreenshot(const std::string&)
    {
    }

    void RenderDevice::executeOnRenderThread(const std::function<void()>& func)
    {
        std::lock_guard lock(executeMutex);
        executeQueue.push(func);
    }

    void RenderDevice::executeAll()
    {
        std::function<void()> func;

        for (;;)
        {
            std::unique_lock lock(executeMutex);
            if (executeQueue.empty()) break;

            func = std::move(executeQueue.front());
            executeQueue.pop();
            lock.unlock();

            if (func) func();
        }
    }
}
