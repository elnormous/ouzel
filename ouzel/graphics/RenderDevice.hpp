// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERDEVICE_HPP
#define OUZEL_GRAPHICS_RENDERDEVICE_HPP

#include <array>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <set>
#include "graphics/Commands.hpp"
#include "graphics/Driver.hpp"
#include "graphics/Filter.hpp"
#include "graphics/Vertex.hpp"
#include "math/Matrix.hpp"
#include "math/Size.hpp"

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
            static const std::array<Vertex::Attribute, 5> VERTEX_ATTRIBUTES;

            struct Event
            {
                enum class Type
                {
                    FRAME
                };

                Type type;
            };

            RenderDevice(Driver initDriver, const std::function<void(const Event&)>& initCallback);
            virtual ~RenderDevice();

            RenderDevice(const RenderDevice&) = delete;
            RenderDevice& operator=(const RenderDevice&) = delete;

            RenderDevice(RenderDevice&&) = delete;
            RenderDevice& operator=(RenderDevice&&) = delete;

            inline Driver getDriver() const { return driver; }

            virtual void process();

            inline uint32_t getSampleCount() const { return sampleCount; }
            inline Filter getTextureFilter() const { return textureFilter; }
            inline uint32_t getMaxAnisotropy() const { return maxAnisotropy; }

            virtual std::vector<Size2U> getSupportedResolutions() const;

            void submitCommandBuffer(CommandBuffer&& commandBuffer)
            {
                std::unique_lock<std::mutex> lock(commandQueueMutex);
                commandQueue.push(std::forward<CommandBuffer>(commandBuffer));
                lock.unlock();
                commandQueueCondition.notify_all();
            }

            inline uint32_t getDrawCallCount() const { return drawCallCount; }

            inline uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            inline uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            inline bool isNPOTTexturesSupported() const { return npotTexturesSupported; }
            inline bool isAnisotropicFilteringSupported() const { return anisotropicFilteringSupported; }
            inline bool isRenderTargetsSupported() const { return renderTargetsSupported; }

            const Matrix4F& getProjectionTransform(bool renderTarget) const
            {
                return renderTarget ? renderTargetProjectionTransform : projectionTransform;
            }

            inline float getFPS() const { return currentFPS; }
            inline float getAccumulatedFPS() const { return accumulatedFPS; }

            void executeOnRenderThread(const std::function<void()>& func);

            uintptr_t getResourceId()
            {
                auto i = deletedResourceIds.begin();

                if (i == deletedResourceIds.end())
                    return ++lastResourceId; // zero is reserved for null resource
                else
                {
                    uintptr_t resourceId = *i;
                    deletedResourceIds.erase(i);
                    return resourceId;
                }
            }

            void deleteResourceId(uintptr_t resourceId)
            {
                deletedResourceIds.insert(resourceId);
            }

        protected:
            virtual void init(Window* newWindow,
                              const Size2U& newSize,
                              uint32_t newSampleCount,
                              Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newSrgb,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newStencil,
                              bool newDebugRenderer);

            void executeAll();

            virtual void generateScreenshot(const std::string& filename);

            Driver driver;
            std::function<void(const Event&)> callback;

            Window* window = nullptr;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            uint32_t sampleCount = 1; // MSAA sample count
            Filter textureFilter = Filter::POINT;
            uint32_t maxAnisotropy = 1;

            bool verticalSync = true;
            bool srgb = false;
            bool depth = false;
            bool stencil = false;
            bool debugRenderer = false;

            // features
            bool npotTexturesSupported:1;
            bool anisotropicFilteringSupported:1;
            bool renderTargetsSupported:1;
            bool clampToBorderSupported:1;
            bool multisamplingSupported:1;

            Matrix4F projectionTransform;
            Matrix4F renderTargetProjectionTransform;

            uint32_t drawCallCount = 0;

            std::queue<CommandBuffer> commandQueue;
            std::mutex commandQueueMutex;
            std::condition_variable commandQueueCondition;

            std::atomic<float> currentFPS{0.0F};
            std::chrono::steady_clock::time_point previousFrameTime;

            float accumulatedTime = 0.0F;
            float currentAccumulatedFPS = 0.0F;
            std::atomic<float> accumulatedFPS{0.0F};

            std::queue<std::function<void()>> executeQueue;
            std::mutex executeMutex;

            uintptr_t lastResourceId = 0;
            std::set<uintptr_t> deletedResourceIds;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_RENDERDEVICE_HPP
