// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "RenderDevice.hpp"
#include "thread/Lock.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDevice::RenderDevice(Renderer::Driver initDriver):
            driver(initDriver),
            projectionTransform(Matrix4::identity()),
            renderTargetProjectionTransform(Matrix4::identity()),
            refillQueue(true),
            currentFPS(0.0F),
            accumulatedFPS(0.0F)
        {
        }

        RenderDevice::~RenderDevice()
        {
        }

        bool RenderDevice::init(Window* newWindow,
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

            return true;
        }

        bool RenderDevice::process()
        {
            std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - previousFrameTime);
            previousFrameTime = currentTime;

            float delta = diff.count() / 1000000000.0F;

            if (delta > 0.0F)
            {
                currentFPS = 1.0F / delta;
            }

            accumulatedTime += delta;
            currentAccumulatedFPS += 1.0F;

            if (accumulatedTime > 1.0F)
            {
                accumulatedFPS = currentAccumulatedFPS;
                accumulatedTime = 0.0F;
                currentAccumulatedFPS = 0.0F;
            }

            std::vector<std::unique_ptr<RenderResource>> deleteResources; // will be cleared at the end of the scope
            {
                Lock lock(resourceMutex);
                deleteResources = std::move(resourceDeleteSet);
            }

#if OUZEL_MULTITHREADED
            Lock lock(commandQueueMutex);
            while (!queueFinished) commandQueueCondition.wait(lock);
#endif

            std::swap(fillBuffer, renderBuffer);

            queueFinished = false;

            // refills the draw queue
            refillQueue = true;

            executeAll();

            processCommands(*renderBuffer);

            return true;
        }

        void RenderDevice::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;
        }

        void RenderDevice::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;
        }

        void RenderDevice::setClearColor(Color color)
        {
            clearColor = color;
        }

        void RenderDevice::setClearDepth(float newClearDepth)
        {
            clearDepth = newClearDepth;
        }

        void RenderDevice::setSize(const Size2& newSize)
        {
            size = newSize;
        }

        std::vector<Size2> RenderDevice::getSupportedResolutions() const
        {
            return std::vector<Size2>();
        }

        void RenderDevice::deleteResource(RenderResource* resource)
        {
            Lock lock(resourceMutex);

            auto resourceIterator = std::find_if(resources.begin(), resources.end(), [resource](const std::unique_ptr<RenderResource>& ptr) {
                return ptr.get() == resource;
            });

            if (resourceIterator != resources.end())
            {
                resourceDeleteSet.push_back(std::move(*resourceIterator));
                resources.erase(resourceIterator);
            }
        }

        void RenderDevice::flushCommands()
        {
#if OUZEL_MULTITHREADED
            Lock lock(commandQueueMutex);
#endif

            //drawCallCount = static_cast<uint32_t>(fillBuffer->size());

            refillQueue = false;
            queueFinished = true;

#if OUZEL_MULTITHREADED
            commandQueueCondition.signal();
#endif
        }

        bool RenderDevice::generateScreenshot(const std::string&)
        {
            return true;
        }

        void RenderDevice::executeOnRenderThread(const std::function<void(void)>& func)
        {
            Lock lock(executeMutex);

            executeQueue.push(func);
        }

        void RenderDevice::executeAll()
        {
            std::function<void(void)> func;

            for (;;)
            {
                {
                    Lock lock(executeMutex);
                    if (executeQueue.empty()) break;

                    func = std::move(executeQueue.front());
                    executeQueue.pop();
                }

                if (func) func();
            }
        }
    } // namespace graphics
} // namespace ouzel
