// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERDEVICE_HPP
#define OUZEL_GRAPHICS_RENDERDEVICE_HPP

#include <array>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <set>
#include "graphics/Commands.hpp"
#include "graphics/Driver.hpp"
#include "graphics/SamplerFilter.hpp"
#include "graphics/Vertex.hpp"
#include "math/Matrix.hpp"
#include "math/Size.hpp"

namespace ouzel
{
    class Window;

    namespace graphics
    {
        class Renderer;

        class RenderDevice
        {
            friend Renderer;
        public:
            static const std::array<Vertex::Attribute, 5> VERTEX_ATTRIBUTES;

            struct Event
            {
                enum class Type
                {
                    Frame
                };

                Type type;
            };

            RenderDevice(Driver initDriver, const std::function<void(const Event&)>& initCallback);
            virtual ~RenderDevice() = default;

            RenderDevice(const RenderDevice&) = delete;
            RenderDevice& operator=(const RenderDevice&) = delete;

            RenderDevice(RenderDevice&&) = delete;
            RenderDevice& operator=(RenderDevice&&) = delete;

            inline auto getDriver() const noexcept { return driver; }

            virtual void process();

            inline auto getSampleCount() const noexcept { return sampleCount; }
            inline auto getTextureFilter() const noexcept { return textureFilter; }
            inline auto getMaxAnisotropy() const noexcept { return maxAnisotropy; }

            virtual std::vector<Size2U> getSupportedResolutions() const;

            void submitCommandBuffer(CommandBuffer&& commandBuffer)
            {
                std::unique_lock<std::mutex> lock(commandQueueMutex);
                commandQueue.push(std::move(commandBuffer));
                lock.unlock();
                commandQueueCondition.notify_all();
            }

            inline auto getDrawCallCount() const noexcept { return drawCallCount; }

            inline auto getAPIMajorVersion() const noexcept { return apiMajorVersion; }
            inline auto getAPIMinorVersion() const noexcept { return apiMinorVersion; }

            inline auto isNPOTTexturesSupported() const noexcept { return npotTexturesSupported; }
            inline auto isAnisotropicFilteringSupported() const noexcept { return anisotropicFilteringSupported; }
            inline auto isRenderTargetsSupported() const noexcept { return renderTargetsSupported; }

            auto& getProjectionTransform(bool renderTarget) const noexcept
            {
                return renderTarget ? renderTargetProjectionTransform : projectionTransform;
            }

            inline float getFPS() const noexcept { return currentFPS; }
            inline float getAccumulatedFPS() const noexcept { return accumulatedFPS; }

            void executeOnRenderThread(const std::function<void()>& func);

            class Resource final
            {
            public:
                Resource() = default;
                Resource(std::uintptr_t initId, const std::function<void(std::uintptr_t)>& initDeleter):
                    id(initId), deleter(initDeleter)
                {
                }

                ~Resource()
                {
                    if (id) deleter(id);
                }

                Resource(const Resource&) = delete;
                Resource& operator=(const Resource&) = delete;

                Resource(Resource&& other) noexcept:
                    id(other.id),
                    deleter(std::move(other.deleter))
                {
                    other.id = 0;
                }

                Resource& operator=(Resource&& other) noexcept
                {
                    if (&other == this) return *this;

                    id = other.id;
                    deleter = std::move(other.deleter);
                    other.id = 0;

                    return *this;
                }

                inline operator std::uintptr_t() const noexcept
                {
                    return id;
                }

            private:
                std::uintptr_t id = 0;
                std::function<void(std::uintptr_t)> deleter;
            };

            Resource createResource()
            {
                auto i = deletedResourceIds.begin();

                if (i == deletedResourceIds.end())
                    return Resource(++lastResourceId, [this](std::uintptr_t id){
                        deletedResourceIds.insert(id);
                    }); // zero is reserved for null resource
                else
                {
                    std::uintptr_t resourceId = *i;
                    deletedResourceIds.erase(i);
                    return Resource(resourceId, [this](std::uintptr_t id){
                        deletedResourceIds.insert(id);
                    });
                }
            }

        protected:
            virtual void init(Window* newWindow,
                              const Size2U& newSize,
                              std::uint32_t newSampleCount,
                              SamplerFilter newTextureFilter,
                              std::uint32_t newMaxAnisotropy,
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

            std::uint16_t apiMajorVersion = 0;
            std::uint16_t apiMinorVersion = 0;

            std::uint32_t sampleCount = 1; // MSAA sample count
            SamplerFilter textureFilter = SamplerFilter::Point;
            std::uint32_t maxAnisotropy = 1;

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
            bool uintIndicesSupported:1;

            Matrix4F projectionTransform = Matrix4F::identity();
            Matrix4F renderTargetProjectionTransform = Matrix4F::identity();

            std::uint32_t drawCallCount = 0;

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

        private:
            std::uintptr_t lastResourceId = 0;
            std::set<std::uintptr_t> deletedResourceIds;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_RENDERDEVICE_HPP
