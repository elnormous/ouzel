// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDevice::RenderDevice(Driver initDriver,
                                   const std::function<void(const Event&)>& initCallback):
            driver(initDriver),
            callback(initCallback),
            projectionTransform(Matrix4::identity()),
            renderTargetProjectionTransform(Matrix4::identity()),
            currentFPS(0.0F),
            accumulatedFPS(0.0F)
        {
        }

        RenderDevice::~RenderDevice()
        {
        }

        void RenderDevice::init(Window* newWindow,
                                const Size2& newSize,
                                uint32_t newSampleCount,
                                Texture::Filter newTextureFilter,
                                uint32_t newMaxAnisotropy,
                                bool newVerticalSync,
                                bool newDepth,
                                bool newDebugRenderer)
        {
            window = newWindow;
            size = newSize;
            sampleCount = newSampleCount;
            textureFilter = newTextureFilter;
            maxAnisotropy = newMaxAnisotropy;
            verticalSync = newVerticalSync;
            depth = newDepth;
            debugRenderer = newDebugRenderer;
            clearColor = Color::BLACK;

            previousFrameTime = std::chrono::steady_clock::now();
        }

        void RenderDevice::process()
        {
            Event event;
            event.type = Event::Type::FRAME;
            callback(event);

            std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - previousFrameTime);
            previousFrameTime = currentTime;

            float delta = diff.count() / 1000000000.0F;

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

        void RenderDevice::setSize(const Size2& newSize)
        {
            size = newSize;
        }

        std::vector<Size2> RenderDevice::getSupportedResolutions() const
        {
            return std::vector<Size2>();
        }

        void RenderDevice::generateScreenshot(const std::string&)
        {
        }

        void RenderDevice::executeOnRenderThread(const std::function<void()>& func)
        {
            std::unique_lock<std::mutex> lock(executeMutex);
            executeQueue.push(func);
        }

        void RenderDevice::executeAll()
        {
            std::function<void()> func;

            for (;;)
            {
                std::unique_lock<std::mutex> lock(executeMutex);
                if (executeQueue.empty()) break;

                func = std::move(executeQueue.front());
                executeQueue.pop();
                lock.unlock();

                if (func) func();
            }
        }
    } // namespace graphics
} // namespace ouzel
