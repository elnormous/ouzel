// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Renderer.h"
#include "core/Engine.h"
#include "Resource.h"
#include "TextureResource.h"
#include "ShaderResource.h"
#include "BlendStateResource.h"
#include "MeshBufferResource.h"
#include "IndexBufferResource.h"
#include "VertexBufferResource.h"
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

            std::set<Resource*> resources;
            {
                std::lock_guard<std::mutex> lock(uploadMutex);
                resources = std::move(uploadSet);
                uploadSet.clear();
            }

            // refills draw and upload queues
            refillDrawQueue = true;

            for (Resource* resource : resources)
            {
                // upload data to GPU
                if (resource->dirty && !resource->upload())
                {
                    return false;
                }
            }

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

        bool Renderer::addDrawCommand(const std::vector<TextureResourcePtr>& textures,
                                      const ShaderResourcePtr& shader,
                                      const std::vector<std::vector<float>>& pixelShaderConstants,
                                      const std::vector<std::vector<float>>& vertexShaderConstants,
                                      const BlendStateResourcePtr& blendState,
                                      const MeshBufferResourcePtr& meshBuffer,
                                      uint32_t indexCount,
                                      DrawMode drawMode,
                                      uint32_t startIndex,
                                      const TextureResourcePtr& renderTarget,
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

            std::lock_guard<std::mutex> lock(uploadMutex);

            for (const TextureResourcePtr& texture : textures)
            {
                if (texture && texture->dirty) uploadSet.insert(texture.get());
            }

            if (shader && shader->dirty) uploadSet.insert(shader.get());
            if (blendState && blendState->dirty) uploadSet.insert(blendState.get());
            if (meshBuffer)
            {
                if (meshBuffer && meshBuffer->dirty) uploadSet.insert(meshBuffer.get());
                if (meshBuffer->indexBuffer && meshBuffer->indexBuffer->dirty) uploadSet.insert(meshBuffer->indexBuffer.get());
                if (meshBuffer->vertexBuffer && meshBuffer->vertexBuffer->dirty) uploadSet.insert(meshBuffer->vertexBuffer.get());
            }
            if (renderTarget && renderTarget->dirty) uploadSet.insert(renderTarget.get());

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
