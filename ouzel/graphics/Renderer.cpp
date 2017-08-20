// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Renderer.hpp"
#include "RenderDevice.hpp"
#include "core/Engine.hpp"
#include "TextureResource.hpp"
#include "ShaderResource.hpp"
#include "BlendStateResource.hpp"
#include "MeshBufferResource.hpp"
#include "BufferResource.hpp"
#include "events/EventHandler.hpp"
#include "events/EventDispatcher.hpp"
#include "core/Window.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        Renderer::Renderer(Driver aDriver):
            driver(aDriver),
            projectionTransform(Matrix4::IDENTITY),
            renderTargetProjectionTransform(Matrix4::IDENTITY),
            refillQueue(true),
            currentFPS(0.0f),
            accumulatedFPS(0.0f)
        {
        }

        Renderer::~Renderer()
        {
            if (device) delete device;
        }

        bool Renderer::init(Window* newWindow,
                            const Size2& newSize,
                            uint32_t newSampleCount,
                            Texture::Filter newTextureFilter,
                            uint32_t newMaxAnisotropy,
                            bool newVerticalSync,
                            bool newDepth,
                            bool newDebugRenderer)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

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

        bool Renderer::upload()
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

            {
                std::lock_guard<std::mutex> lock(uploadMutex);

                if (dirty && !upload())
                {
                    return false;
                }
            }

            std::vector<DrawCommand> drawCommands;
            {
#if OUZEL_MULTITHREADED
                std::unique_lock<std::mutex> lock(drawQueueMutex);

                while (!queueFinished)
                {
                    queueCondition.wait(lock);
                }
#endif

                drawCommands = std::move(drawQueue);
                drawQueue.reserve(drawCommands.size());

                queueFinished = false;
            }

            std::vector<std::unique_ptr<Resource>> deleteResources;
            {
                std::lock_guard<std::mutex> lock(resourceMutex);
                deleteResources = std::move(resourceDeleteSet);
            }

            // refills draw and upload queues
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

        void Renderer::setClearColorBuffer(bool clear)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            clearColorBuffer = clear;
            dirty = true;
        }

        void Renderer::setClearDepthBuffer(bool clear)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            clearDepthBuffer = clear;
            dirty = true;
        }

        void Renderer::setClearColor(Color color)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            clearColor = color;
            dirty = true;
        }

        void Renderer::setClearDepth(float clear)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            clearDepth = clear;
            dirty = true;
        }

        void Renderer::setSize(const Size2& newSize)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (size != newSize)
            {
                size = newSize;
                dirty = true;
            }
        }

        std::vector<Size2> Renderer::getSupportedResolutions() const
        {
            return std::vector<Size2>();
        }

        void Renderer::deleteResource(Resource* resource)
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            auto i = std::find_if(resources.begin(), resources.end(), [resource](const std::unique_ptr<Resource>& ptr) {
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
                                      bool scissorTest,
                                      const Rectangle& scissorRectangle,
                                      CullMode cullMode)
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

            if (!meshBuffer || !meshBuffer->getIndexBuffer() || !meshBuffer->getVertexBuffer())
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
                indexCount,
                drawMode,
                startIndex,
                renderTarget ? renderTarget->getResource() : nullptr,
                viewport,
                depthWrite,
                depthTest,
                wireframe,
                scissorTest,
                scissorRectangle,
                cullMode
            });

            return true;
        }

        void Renderer::flushCommands()
        {
            refillQueue = false;

            {
                std::lock_guard<std::mutex> lock(drawQueueMutex);
                queueFinished = true;
                drawCallCount = static_cast<uint32_t>(drawQueue.size());
            }

#if OUZEL_MULTITHREADED
            queueCondition.notify_one();
#endif
        }

        bool Renderer::saveScreenshot(const std::string& filename)
        {
            executeOnRenderThread(std::bind(&Renderer::generateScreenshot, this, filename));

            return true;
        }

        bool Renderer::generateScreenshot(const std::string&)
        {
            return true;
        }

        void Renderer::executeOnRenderThread(const std::function<void(void)>& func)
        {
            std::lock_guard<std::mutex> lock(executeMutex);

            executeQueue.push(func);
        }

        void Renderer::executeAll()
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
