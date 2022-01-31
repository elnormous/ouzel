// Ouzel by Elviss Strazdins

#include <algorithm>
#include "RenderDevice.hpp"
#include "stb_image_write.h"

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
            currentFps = 1.0F / delta;

        accumulatedTime += delta;
        currentAccumulatedFps += 1.0F;

        if (accumulatedTime > 1.0F)
        {
            accumulatedFps = currentAccumulatedFps;
            accumulatedTime = 0.0F;
            currentAccumulatedFps = 0.0F;
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
        eventQueueCondition.wait(lock, [this]() noexcept { return !eventQueue.empty(); });
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
        std::scoped_lock lock{executeMutex};
        executeQueue.push(func);
    }

    void RenderDevice::executeAll()
    {
        std::function<void()> func;

        for (;;)
        {
            std::unique_lock lock{executeMutex};
            if (executeQueue.empty()) break;

            func = std::move(executeQueue.front());
            executeQueue.pop();
            lock.unlock();

            if (func) func();
        }
    }

    void RenderDevice::saveScreenshot(const std::string& filename,
                                      std::size_t width,
                                      std::size_t height,
                                      std::size_t pixelSize,
                                      const void *data,
                                      std::size_t stride)
    {
        if (!stbi_write_png(filename.c_str(),
                            static_cast<int>(width),
                            static_cast<int>(height),
                            static_cast<int>(pixelSize),
                            data,
                            static_cast<int>(stride)))
            throw std::runtime_error{"Failed to save image to file"};
    }
}
