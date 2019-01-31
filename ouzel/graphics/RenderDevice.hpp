// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERDEVICE_HPP
#define OUZEL_GRAPHICS_RENDERDEVICE_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <set>
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

            RenderDevice(Driver initDriver, const std::function<void(const Event&)>& initCallback);
            virtual ~RenderDevice();

            RenderDevice(const RenderDevice&) = delete;
            RenderDevice& operator=(const RenderDevice&) = delete;

            RenderDevice(RenderDevice&&) = delete;
            RenderDevice& operator=(RenderDevice&&) = delete;

            inline Driver getDriver() const { return driver; }

            virtual void process();

            inline const Size2<uint32_t>& getSize() const { return size; }
            inline uint32_t getSampleCount() const { return sampleCount; }
            inline Texture::Filter getTextureFilter() const { return textureFilter; }
            inline uint32_t getMaxAnisotropy() const { return maxAnisotropy; }

            virtual std::vector<Size2<uint32_t>> getSupportedResolutions() const;

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

            const Matrix4<float>& getProjectionTransform(bool renderTarget) const
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
                              const Size2<uint32_t>& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newStencil,
                              bool newDebugRenderer);

            void executeAll();
            virtual void setSize(const Size2<uint32_t>& newSize);

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
            bool stencil = false;
            bool debugRenderer = false;

            bool npotTexturesSupported = true;
            bool anisotropicFilteringSupported = true;
            bool renderTargetsSupported = true;

            Matrix4<float> projectionTransform;
            Matrix4<float> renderTargetProjectionTransform;

            Size2<uint32_t> size;

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
