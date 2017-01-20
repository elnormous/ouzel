// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Renderer.h"
#include "core/Engine.h"
#include "Texture.h"
#include "Shader.h"
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
        Renderer::Renderer(Driver aDriver):
            driver(aDriver),
            refillDrawQueue(true),
            projectionTransform(Matrix4::IDENTITY),
            renderTargetProjectionTransform(Matrix4::IDENTITY),
            clearColor(Color::BLACK),
            dirty(false)
        {
        }

        Renderer::~Renderer()
        {
        }

        void Renderer::free()
        {
            activeDrawQueue.clear();
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

            return true;
        }

        bool Renderer::update()
        {
            return true;
        }

        bool Renderer::present()
        {
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

            {
                std::unique_lock<std::mutex> lock(drawQueueMutex);

                if (!activeDrawQueueFinished)
                {
                    drawQueueCondition.wait(lock);
                }

                drawQueue = std::move(activeDrawQueue);
                activeDrawQueue.reserve(drawQueue.size());

                activeDrawQueueFinished = false;
            }

            std::set<ResourcePtr> resources;
            {
                std::lock_guard<std::mutex> lock(updateMutex);
                resources = std::move(updateSet);
                updateSet.clear();
            }

            for (const ResourcePtr& resource : resources)
            {
                // upload data to GPU
                if (!resource->upload())
                {
                    return false;
                }
            }

            // refills draw and update queues
            refillDrawQueue = true;

            ++currentFrame;

            if (!generateScreenshots())
            {
                return false;
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

        bool Renderer::addDrawCommand(const std::vector<TexturePtr>& textures,
                                      const ShaderPtr& shader,
                                      const std::vector<std::vector<float>>& pixelShaderConstants,
                                      const std::vector<std::vector<float>>& vertexShaderConstants,
                                      const BlendStatePtr& blendState,
                                      const MeshBufferPtr& meshBuffer,
                                      uint32_t indexCount,
                                      DrawMode drawMode,
                                      uint32_t startIndex,
                                      const TexturePtr& renderTarget,
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
                depthWrite,
                depthTest,
                wireframe,
                scissorTestEnabled,
                scissorTest
            });

            std::lock_guard<std::mutex> lock(updateMutex);

            for (const TexturePtr& texture : textures)
            {
                if (texture && texture->dirty) updateSet.insert(texture);
            }

            if (shader && shader->dirty) updateSet.insert(shader);
            if (blendState && blendState->dirty) updateSet.insert(blendState);
            if (meshBuffer)
            {
                if (meshBuffer && meshBuffer->dirty) updateSet.insert(meshBuffer);
                if (meshBuffer->indexBuffer && meshBuffer->indexBuffer->dirty) updateSet.insert(meshBuffer->indexBuffer);
                if (meshBuffer->vertexBuffer && meshBuffer->vertexBuffer->dirty) updateSet.insert(meshBuffer->vertexBuffer);
            }
            if (renderTarget && renderTarget->dirty) updateSet.insert(renderTarget);

            return true;
        }

        void Renderer::flushDrawCommands()
        {
            refillDrawQueue = false;

            {
                std::lock_guard<std::mutex> lock(drawQueueMutex);
                activeDrawQueueFinished = true;
                drawCallCount = static_cast<uint32_t>(activeDrawQueue.size());
            }
            drawQueueCondition.notify_one();
        }

        bool Renderer::saveScreenshot(const std::string& filename)
        {
            std::lock_guard<std::mutex> lock(screenshotMutex);

            screenshotQueue.push(filename);

            return true;
        }

        bool Renderer::generateScreenshots()
        {
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

        bool Renderer::generateScreenshot(const std::string&)
        {
            return true;
        }
    } // namespace graphics
} // namespace ouzel
