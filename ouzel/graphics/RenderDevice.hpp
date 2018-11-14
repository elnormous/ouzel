// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERDEVICE_HPP
#define OUZEL_GRAPHICS_RENDERDEVICE_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>
#include "graphics/Commands.hpp"
#include "graphics/Driver.hpp"
#include "graphics/Texture.hpp"
#include "graphics/Vertex.hpp"
#include "math/Matrix4.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    class Window;
    
    namespace graphics
    {
        class Renderer;
        class BlendState;
        class Buffer;
        class Shader;

        class RenderDevice
        {
            friend Renderer;
            friend BlendState;
            friend Buffer;
            friend Shader;
            friend Texture;
        public:
            struct Event
            {
                enum class Type
                {
                    FRAME
                };

                Type type;
            };

            virtual ~RenderDevice();

            RenderDevice(const RenderDevice&) = delete;
            RenderDevice& operator=(const RenderDevice&) = delete;

            RenderDevice(RenderDevice&&) = delete;
            RenderDevice& operator=(RenderDevice&&) = delete;

            inline Driver getDriver() const { return driver; }

            virtual void process();

            inline const Size2& getSize() const { return size; }
            inline uint32_t getSampleCount() const { return sampleCount; }
            inline Texture::Filter getTextureFilter() const { return textureFilter; }
            inline uint32_t getMaxAnisotropy() const { return maxAnisotropy; }

            virtual std::vector<Size2> getSupportedResolutions() const;

            void submitCommandBuffer(CommandBuffer&& commandBuffer)
            {
                {
                    std::unique_lock<std::mutex> lock(commandQueueMutex);
                    commandQueue.push(std::forward<CommandBuffer>(commandBuffer));
                }
                commandQueueCondition.notify_all();
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

            void executeOnRenderThread(const std::function<void()>& func);

            uintptr_t getResourceId()
            {
                if (deletedResourceIds.empty())
                    return ++lastResourceId; // zero is reserved for null resource
                else
                {
                    uintptr_t resourceId = deletedResourceIds.front();
                    deletedResourceIds.pop();
                    return resourceId;
                }
            }

            void deleteResourceId(uintptr_t resourceId)
            {
                deletedResourceIds.push(resourceId);
            }

        protected:
            explicit RenderDevice(Driver initDriver,
                                  const std::function<void(const Event&)>& initCallback);

            virtual void init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer);

            void executeAll();
            virtual void setSize(const Size2& newSize);

            virtual void generateScreenshot(const std::string& filename);

            Driver driver;
            std::function<void(const Event&)> callback;

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

            uint32_t drawCallCount = 0;

            std::queue<CommandBuffer> commandQueue;
            std::mutex commandQueueMutex;
            std::condition_variable commandQueueCondition;

            std::atomic<float> currentFPS;
            std::chrono::steady_clock::time_point previousFrameTime;

            float accumulatedTime = 0.0F;
            float currentAccumulatedFPS = 0.0F;
            std::atomic<float> accumulatedFPS;

            std::queue<std::function<void()>> executeQueue;
            std::mutex executeMutex;

            uintptr_t lastResourceId = 0;
            std::queue<uintptr_t> deletedResourceIds;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_RENDERDEVICE_HPP
