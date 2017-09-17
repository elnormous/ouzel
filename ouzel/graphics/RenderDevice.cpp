// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "RenderDevice.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDevice::RenderDevice(Renderer::Driver aDriver):
            driver(aDriver),
            projectionTransform(Matrix4::IDENTITY),
            renderTargetProjectionTransform(Matrix4::IDENTITY),
            refillQueue(true),
            currentFPS(0.0f),
            accumulatedFPS(0.0f)
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
                                bool newExclusiveFullscreen,
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
            exclusiveFullscreen = newExclusiveFullscreen;
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

            float delta = diff.count() / 1000000000.0f;

            if (delta > 0.0f)
            {
                currentFPS = 1.0f / delta;
            }

            accumulatedTime += delta;
            currentAccumulatedFPS += 1.0f;

            if (accumulatedTime > 1.0f)
            {
                accumulatedFPS = currentAccumulatedFPS;
                accumulatedTime = 0.0f;
                currentAccumulatedFPS = 0.0f;
            }

            std::vector<DrawCommand> drawCommands;
            {
#if OUZEL_MULTITHREADED
                std::unique_lock<std::mutex> lock(drawQueueMutex);

                while (!queueFinished)
                {
                    queueCondition.wait_for(lock, std::chrono::milliseconds(16));
                }
#endif

                drawCommands = drawQueue;
                drawQueue.clear();

                queueFinished = false;
            }

            std::vector<std::unique_ptr<Resource>> deleteResources;
            {
                std::lock_guard<std::mutex> lock(resourceMutex);
                deleteResources = std::move(resourceDeleteSet);
            }

            // refills the draw queue
            refillQueue = true;

            executeAll();

            ++currentFrame;

            if (!draw(drawCommands))
            {
                return false;
            }

            deleteResources.clear(); // delete all resources in delete set

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

        void RenderDevice::deleteResource(Resource* resource)
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            auto resourceIterator = std::find_if(resources.begin(), resources.end(), [resource](const std::unique_ptr<Resource>& ptr) {
                return ptr.get() == resource;
            });

            if (resourceIterator != resources.end())
            {
                resourceDeleteSet.push_back(std::move(*resourceIterator));
                resources.erase(resourceIterator);
            }
        }

        bool RenderDevice::addDrawCommand(const DrawCommand& drawCommand)
        {
            std::lock_guard<std::mutex> lock(drawQueueMutex);

            drawQueue.push_back(drawCommand);

            return true;
        }

        void RenderDevice::flushCommands()
        {
            std::lock_guard<std::mutex> lock(drawQueueMutex);
            refillQueue = false;

            queueFinished = true;
            drawCallCount = static_cast<uint32_t>(drawQueue.size());

#if OUZEL_MULTITHREADED
            queueCondition.notify_one();
#endif
        }

        bool RenderDevice::generateScreenshot(const std::string&)
        {
            return true;
        }

        void RenderDevice::executeOnRenderThread(const std::function<void(void)>& func)
        {
            std::lock_guard<std::mutex> lock(executeMutex);

            executeQueue.push(func);
        }

        void RenderDevice::executeAll()
        {
            std::function<void(void)> func;

            for (;;)
            {
                {
                    std::lock_guard<std::mutex> lock(executeMutex);

                    if (executeQueue.empty())
                    {
                        break;
                    }

                    func = std::move(executeQueue.front());
                    executeQueue.pop();
                }

                if (func)
                {
                    func();
                }
            }
        }
    } // namespace graphics
} // namespace ouzel
