// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Renderer.h"
#include "core/Engine.h"
#include "Texture.h"
#include "Shader.h"
#include "RenderTarget.h"
#include "BlendState.h"
#include "MeshBuffer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "events/EventHandler.h"
#include "events/EventDispatcher.h"
#include "core/Window.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        Renderer::Renderer(Driver pDriver):
            driver(pDriver), clearColor(Color::BLACK), clear(true), activeDrawQueueFinished(false), refillDrawQueue(true)
        {
        }

        Renderer::~Renderer()
        {
        }

        void Renderer::free()
        {
            activeDrawQueue.clear();
            ready = false;
        }

        bool Renderer::init(Window* newWindow,
                            uint32_t newSampleCount,
                            TextureFilter newTextureFilter,
                            PixelFormat newBackBufferFormat,
                            bool newVerticalSync)
        {
            window = newWindow;
            size = window->getSize() * window->getContentScale();
            sampleCount = newSampleCount;
            textureFilter = newTextureFilter;
            backBufferFormat = newBackBufferFormat;
            verticalSync = newVerticalSync;

            ready = true;

            return true;
        }

        bool Renderer::present()
        {
            ++currentFrame;

            if (activeDrawQueueFinished)
            {
                drawQueue = std::move(activeDrawQueue);
                activeDrawQueue.reserve(drawQueue.size());
                drawCallCount = static_cast<uint32_t>(drawQueue.size());

                std::set<ResourcePtr> resources;

                {
                    std::lock_guard<std::mutex> lock(updateMutex);
                    resources = std::move(updateSet);
                    updateSet.clear();

                    for (const ResourcePtr& resource : resources)
                    {
                        // prepare data for upload
                        resource->update();
                    }
                }

                for (const ResourcePtr& resource : resources)
                {
                    // upload data to GPU
                    if (!resource->upload())
                    {
                        return false;
                    }
                }

                activeDrawQueueFinished = false;
                refillDrawQueue = true;
            }

            return true;
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
            }
        }

        std::vector<Size2> Renderer::getSupportedResolutions() const
        {
            return std::vector<Size2>();
        }

        bool Renderer::addDrawCommand(const std::vector<TexturePtr>& textures,
                                      const ShaderPtr& shader,
                                      const std::vector<std::vector<float>>& pixelShaderConstants,
                                      const std::vector<std::vector<float>>& vertexShaderConstants,
                                      const BlendStatePtr& blendState,
                                      const MeshBufferPtr& meshBuffer,
                                      uint32_t indexCount,
                                      DrawMode drawMode,
                                      uint32_t startIndex,
                                      const RenderTargetPtr& renderTarget,
                                      const Rectangle& viewport,
                                      bool wireframe,
                                      bool scissorTestEnabled,
                                      const Rectangle& scissorTest)
        {

            if (!shader)
            {
                Log(Log::Level::ERR) << "No shader passed to render queue";
                return false;
            }

            if (!meshBuffer || !meshBuffer->getIndexBuffer() || !meshBuffer->getVertexBuffer() ||
                shader->getVertexAttributes() != meshBuffer->getVertexBuffer()->getVertexAttributes())
            {
                Log(Log::Level::ERR) << "Invalid mesh buffer passed to render queue";
                return false;
            }

            activeDrawQueue.push_back({
                textures,
                shader,
                pixelShaderConstants,
                vertexShaderConstants,
                blendState,
                meshBuffer,
                (indexCount > 0) ? indexCount : meshBuffer->getIndexBuffer()->getIndexCount() - startIndex,
                drawMode,
                startIndex,
                renderTarget,
                viewport,
                wireframe,
                scissorTestEnabled,
                scissorTest
            });

            return true;
        }

        void Renderer::flushDrawCommands()
        {
            refillDrawQueue = false;
            activeDrawQueueFinished = true;
        }

        bool Renderer::saveScreenshot(const std::string& filename)
        {
            std::lock_guard<std::mutex> lock(screenshotMutex);

            screenshotQueue.push(filename);

            return true;
        }

        void Renderer::scheduleUpdate(const ResourcePtr& resource)
        {
            std::lock_guard<std::mutex> lock(updateMutex);

            updateSet.insert(resource);
        }
    } // namespace graphics
} // namespace ouzel
