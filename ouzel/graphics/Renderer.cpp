// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Renderer.h"
#include "core/Engine.h"
#include "Resource.h"
#include "TextureResource.h"
#include "ShaderResource.h"
#include "BlendStateResource.h"
#include "MeshBufferResource.h"
#include "BufferResource.h"
#include "events/EventHandler.h"
#include "events/EventDispatcher.h"
#include "core/Window.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        Renderer::Renderer(Driver aDriver):
            driver(aDriver),
            projectionTransform(Matrix4::IDENTITY),
            renderTargetProjectionTransform(Matrix4::IDENTITY),
            clearColor(Color::BLACK),
            refillDrawQueue(true),
            currentFPS(0.0f),
            accumulatedFPS(0.0f),
            dirty(false)
        {
        }

        Renderer::~Renderer()
        {
        }

        bool Renderer::init(Window* newWindow,
                            const Size2& newSize,
                            uint32_t newSampleCount,
                            Texture::Filter newTextureFilter,
                            PixelFormat newBackBufferFormat,
                            bool newVerticalSync,
                            bool newDepth)
        {
            window = newWindow;
            size = newSize;
            sampleCount = newSampleCount;
            textureFilter = newTextureFilter;
            backBufferFormat = newBackBufferFormat;
            verticalSync = newVerticalSync;
            depth = newDepth;

            previousFrameTime = std::chrono::steady_clock::now();

            return true;
        }

        bool Renderer::update()
        {
            return true;
        }

        bool Renderer::process()
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

            if (dirty)
            {
                uploadData.size = size;
                uploadData.clearColor = clearColor;
                uploadData.clearColorBuffer = clearColorBuffer;
                uploadData.clearDepthBuffer = clearDepthBuffer;

                if (!update())
                {
                    return false;
                }

                dirty = false;
            }

            std::vector<DrawCommand> drawCommands;
            {
#if OUZEL_MULTITHREADED
                std::unique_lock<std::mutex> lock(drawQueueMutex);

                while (!drawQueueFinished)
                {
                    drawQueueCondition.wait(lock);
                }
#endif

                drawCommands = std::move(drawQueue);
                drawQueue.reserve(drawCommands.size());

                drawQueueFinished = false;
            }

            std::set<Resource*> uploadResources;
            std::vector<std::unique_ptr<Resource>> deleteResources;
            {
                std::lock_guard<std::mutex> lock(resourceMutex);
                uploadResources = std::move(resourceUploadSet);
                deleteResources = std::move(resourceDeleteSet);
            }

            // refills draw and upload queues
            refillDrawQueue = true;

            for (Resource* resource : uploadResources)
            {
                // upload data to GPU
                if (!resource->upload())
                {
                    return false;
                }
            }

            ++currentFrame;

            if (!draw(drawCommands))
            {
                return false;
            }

            deleteResources.clear(); // delete all resources in delete set

            for (;;)
            {
                std::string filename;

                {
                    std::lock_guard<std::mutex> lock(screenshotMutex);

                    if (screenshotQueue.empty()) break;

                    filename = screenshotQueue.front();
                    screenshotQueue.pop();
                }

                if (!generateScreenshot(filename))
                {
                    return false;
                }
            }

            return true;
        }

        void Renderer::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;
            dirty = true;
        }

        void Renderer::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;
            dirty = true;
        }

        void Renderer::setClearColor(Color color)
        {
            clearColor = color;
            dirty = true;
        }

        void Renderer::setSize(const Size2& newSize)
        {
            if (size != newSize)
            {
                size = newSize;

                Event event;
                event.type = Event::Type::WINDOW_RESOLUTION_CHANGE;

                event.windowEvent.window = window;
                event.windowEvent.size = size;

                sharedEngine->getEventDispatcher()->postEvent(event);

                dirty = true;
            }
        }

        std::vector<Size2> Renderer::getSupportedResolutions() const
        {
            return std::vector<Size2>();
        }

        void Renderer::uploadResource(Resource* resource)
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            resourceUploadSet.insert(resource);
        }

        void Renderer::deleteResource(Resource* resource)
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            std::vector<std::unique_ptr<Resource>>::iterator i = std::find_if(resources.begin(), resources.end(), [resource](const std::unique_ptr<Resource>& ptr) {
                return ptr.get() == resource;
            });

            if (i != resources.end())
            {
                resourceDeleteSet.push_back(std::move(*i));
                resources.erase(i);
            }
        }

        bool Renderer::addDrawCommand(const std::vector<std::shared_ptr<Texture>>& textures,
                                      const std::shared_ptr<Shader>& shader,
                                      const std::vector<std::vector<float>>& pixelShaderConstants,
                                      const std::vector<std::vector<float>>& vertexShaderConstants,
                                      const std::shared_ptr<BlendState>& blendState,
                                      const std::shared_ptr<MeshBuffer>& meshBuffer,
                                      uint32_t indexCount,
                                      DrawMode drawMode,
                                      uint32_t startIndex,
                                      const std::shared_ptr<Texture>& renderTarget,
                                      const Rectangle& viewport,
                                      bool depthWrite,
                                      bool depthTest,
                                      bool wireframe,
                                      bool scissorTestEnabled,
                                      const Rectangle& scissorTest)
        {

            if (!shader)
            {
                Log(Log::Level::ERR) << "No shader passed to render queue";
                return false;
            }

            if (!blendState)
            {
                Log(Log::Level::ERR) << "No blend state passed to render queue";
                return false;
            }

            if (!meshBuffer || !meshBuffer->getIndexBuffer() || !meshBuffer->getVertexBuffer() ||
                shader->getVertexSize() != meshBuffer->getVertexSize())
            {
                Log(Log::Level::ERR) << "Invalid mesh buffer passed to render queue";
                return false;
            }

            std::vector<TextureResource*> drawTextures;

            for (const std::shared_ptr<Texture>& texture : textures)
            {
                if (texture) drawTextures.push_back(texture->getResource());
            }

            drawQueue.push_back({
                drawTextures,
                shader->getResource(),
                pixelShaderConstants,
                vertexShaderConstants,
                blendState->getResource(),
                meshBuffer->getResource(),
                (indexCount > 0) ? indexCount : (meshBuffer->getIndexBuffer()->getSize() / meshBuffer->getIndexSize()) - startIndex,
                drawMode,
                startIndex,
                renderTarget ? renderTarget->getResource() : nullptr,
                viewport,
                depthWrite,
                depthTest,
                wireframe,
                scissorTestEnabled,
                scissorTest
            });

            return true;
        }

        void Renderer::flushDrawCommands()
        {
            refillDrawQueue = false;

            {
                std::lock_guard<std::mutex> lock(drawQueueMutex);
                drawQueueFinished = true;
                drawCallCount = static_cast<uint32_t>(drawQueue.size());
            }

#if OUZEL_MULTITHREADED
            drawQueueCondition.notify_one();
#endif
        }

        bool Renderer::saveScreenshot(const std::string& filename)
        {
            std::lock_guard<std::mutex> lock(screenshotMutex);

            screenshotQueue.push(filename);

            return true;
        }
        
        bool Renderer::generateScreenshot(const std::string&)
        {
            return true;
        }
    } // namespace graphics
} // namespace ouzel
