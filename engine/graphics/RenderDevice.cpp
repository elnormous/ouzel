// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "RenderDevice.hpp"

namespace ouzel::graphics
{
    const std::array<Vertex::Attribute, 5> RenderDevice::VERTEX_ATTRIBUTES{
        Vertex::Attribute{Vertex::Attribute::Usage::position, DataType::float32Vector3},
        Vertex::Attribute{Vertex::Attribute::Usage::color, DataType::unsignedByteVector4Norm},
        Vertex::Attribute{Vertex::Attribute::Usage::textureCoordinates0, DataType::float32Vector2},
        Vertex::Attribute{Vertex::Attribute::Usage::textureCoordinates1, DataType::float32Vector2},
        Vertex::Attribute{Vertex::Attribute::Usage::normal, DataType::float32Vector3}
    };

    RenderDevice::RenderDevice(Driver initDriver,
                               core::Window& initWindow,
                               const std::function<void(const Event&)>& initCallback):
        driver(initDriver),
        window(initWindow),
        callback(initCallback),
        npotTexturesSupported(false),
        anisotropicFilteringSupported(false),
        renderTargetsSupported(false),
        clampToBorderSupported(false),
        multisamplingSupported(false),
        uintIndicesSupported(false)
    {
    }

    void RenderDevice::init(const Size2U&,
                            const Settings& settings)
    {
        sampleCount = settings.sampleCount;
        srgb = settings.srgb;
        verticalSync = settings.verticalSync;
        depth = settings.depth;
        stencil = settings.stencil;
        debugRenderer = settings.debugRenderer;

        previousFrameTime = std::chrono::steady_clock::now();
    }

    void RenderDevice::process()
    {
        Event event;
        event.type = Event::Type::frame;
        callback(event);

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
    }

    std::vector<Size2U> RenderDevice::getSupportedResolutions() const
    {
        return std::vector<Size2U>();
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
