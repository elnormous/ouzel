// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/Renderer.hpp"
#include "graphics/Vertex.hpp"
#include "graphics/CommandBuffer.hpp"
#include "graphics/BlendStateResource.hpp"
#include "graphics/BufferResource.hpp"
#include "graphics/RenderTargetResource.hpp"
#include "graphics/ShaderResource.hpp"
#include "graphics/TextureResource.hpp"
#include "thread/Condition.hpp"
#include "thread/Mutex.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDevice
        {
            friend Renderer;
            friend BlendState;
            friend Buffer;
            friend Shader;
            friend Texture;
        public:
            virtual ~RenderDevice();

            RenderDevice(const RenderDevice&) = delete;
            RenderDevice& operator=(const RenderDevice&) = delete;

            RenderDevice(RenderDevice&&) = delete;
            RenderDevice& operator=(RenderDevice&&) = delete;

            inline Renderer::Driver getDriver() const { return driver; }

            virtual void setClearColorBuffer(bool clear);
            inline bool getClearColorBuffer() const { return clearColorBuffer; }

            virtual void setClearDepthBuffer(bool clear);
            inline bool getClearDepthBuffer() const { return clearDepthBuffer; }

            virtual void setClearColor(Color color);
            inline Color getClearColor() const { return clearColor; }

            virtual void setClearDepth(float newClearDepth);
            inline float getClearDepth() const { return clearDepth; }

            virtual bool process();

            inline const Size2& getSize() const { return size; }
            inline uint32_t getSampleCount() const { return sampleCount; }
            inline Texture::Filter getTextureFilter() const { return textureFilter; }
            inline uint32_t getMaxAnisotropy() const { return maxAnisotropy; }

            virtual std::vector<Size2> getSupportedResolutions() const;

            inline bool getRefillQueue() const { return refillQueue; }

            template <class T> bool addCommand(const T& command)
            {
                fillBuffer->push(command);

                return true;
            }

            void flushCommands();

            Vector2 convertScreenToNormalizedLocation(const Vector2& position)
            {
                return Vector2(position.x / size.width,
                               1.0F - (position.y / size.height));
            }

            Vector2 convertNormalizedToScreenLocation(const Vector2& position)
            {
                return Vector2(position.x * size.width,
                               (1.0F - position.y) * size.height);
            }

            inline uint32_t getDrawCallCount() const { return drawCallCount; }

            inline uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            inline uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            inline bool isNPOTTexturesSupported() const { return npotTexturesSupported; }
            inline bool isMultisamplingSupported() const { return multisamplingSupported; }
            inline bool isAnisotropicFilteringSupported() const { return anisotropicFilteringSupported; }
            inline bool isRenderTargetsSupported() const { return renderTargetsSupported; }

            const Matrix4& getProjectionTransform(bool renderTarget) const
            {
                return renderTarget ? renderTargetProjectionTransform : projectionTransform;
            }

            inline float getFPS() const { return currentFPS; }
            inline float getAccumulatedFPS() const { return accumulatedFPS; }

            void executeOnRenderThread(const std::function<void(void)>& func);

        protected:
            explicit RenderDevice(Renderer::Driver initDriver);

            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer);

            void executeAll();
            virtual void setSize(const Size2& newSize);

            virtual BlendStateResource* createBlendState() = 0;
            virtual BufferResource* createBuffer() = 0;
            virtual RenderTargetResource* createRenderTarget() = 0;
            virtual ShaderResource* createShader() = 0;
            virtual TextureResource* createTexture() = 0;
            virtual void deleteResource(RenderResource* resource);

            virtual bool processCommands(CommandBuffer& commands) = 0;
            virtual bool generateScreenshot(const std::string& filename);

            Renderer::Driver driver;

            Window* window = nullptr;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            uint32_t sampleCount = 1; // MSAA sample count
            Texture::Filter textureFilter = Texture::Filter::POINT;
            uint32_t maxAnisotropy = 1;

            bool verticalSync = true;
            bool depth = false;
            bool debugRenderer = false;

            bool npotTexturesSupported = true;
            bool multisamplingSupported = true;
            bool anisotropicFilteringSupported = true;
            bool renderTargetsSupported = true;

            Matrix4 projectionTransform;
            Matrix4 renderTargetProjectionTransform;

            Size2 size;
            Color clearColor;
            float clearDepth = 1.0;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;

            Mutex resourceMutex;
            std::vector<std::unique_ptr<RenderResource>> resources;
            std::vector<std::unique_ptr<RenderResource>> resourceDeleteSet;

            uint32_t drawCallCount = 0;

            CommandBuffer commandBuffers[2];
            CommandBuffer* fillBuffer = &commandBuffers[0];
            CommandBuffer* renderBuffer = &commandBuffers[1];

            Mutex commandQueueMutex;
            Condition commandQueueCondition;

            bool queueFinished = false;
            std::atomic<bool> refillQueue;

            std::atomic<float> currentFPS;
            std::chrono::steady_clock::time_point previousFrameTime;

            float accumulatedTime = 0.0F;
            float currentAccumulatedFPS = 0.0F;
            std::atomic<float> accumulatedFPS;

            std::queue<std::function<void(void)>> executeQueue;
            Mutex executeMutex;
        };
    } // namespace graphics
} // namespace ouzel
