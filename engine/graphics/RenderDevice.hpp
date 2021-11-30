// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_RENDERDEVICE_HPP
#define OUZEL_GRAPHICS_RENDERDEVICE_HPP

#include <array>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <set>
#include "Commands.hpp"
#include "Driver.hpp"
#include "SamplerFilter.hpp"
#include "Settings.hpp"
#include "Vertex.hpp"
#include "../math/Matrix.hpp"
#include "../math/Size.hpp"
#include "../utils/Log.hpp"

namespace ouzel::core
{
    class Window;
}

namespace ouzel::graphics
{
    class Graphics;

    struct ApiVersion final
    {
        constexpr ApiVersion() noexcept = default;
        constexpr ApiVersion(std::uint16_t majorVersion, std::uint16_t minorVersion) noexcept:
            v{majorVersion, minorVersion} {}

        [[nodiscard]] constexpr bool operator==(const ApiVersion& other) const noexcept
        {
            return v[0] == other.v[0] && v[1] == other.v[1];
        }

        [[nodiscard]] constexpr bool operator>(const ApiVersion& other) const noexcept
        {
            return (v[0] == other.v[0]) ? (v[1] > other.v[1]) : (v[0] > other.v[0]);
        }

        [[nodiscard]] constexpr bool operator>=(const ApiVersion& other) const noexcept
        {
            return (v[0] == other.v[0]) ? (v[1] >= other.v[1]) : (v[0] > other.v[0]);
        }

        std::uint16_t v[2]{0, 0};
    };

    inline Log& operator<<(Log& log, const ApiVersion& val)
    {
        log << val.v[0] << '.' << val.v[1];
        return log;
    }

    class RenderDevice
    {
        friend Graphics;
    public:
        static constexpr std::array<Vertex::Attribute, 5> vertexAttributes{
            Vertex::Attribute{{Vertex::Attribute::Usage::position}, DataType::float32Vector3},
            Vertex::Attribute{{Vertex::Attribute::Usage::color}, DataType::unsignedByteVector4Norm},
            Vertex::Attribute{{Vertex::Attribute::Usage::textureCoordinates, 0U}, DataType::float32Vector2},
            Vertex::Attribute{{Vertex::Attribute::Usage::textureCoordinates, 1U}, DataType::float32Vector2},
            Vertex::Attribute{{Vertex::Attribute::Usage::normal}, DataType::float32Vector3}
        };

        struct Event final
        {
            enum class Type
            {
                frame
            };

            Type type;
        };

        RenderDevice(Driver initDriver,
                     const Settings& settings,
                     core::Window& initWindow);
        virtual ~RenderDevice() = default;

        RenderDevice(const RenderDevice&) = delete;
        RenderDevice& operator=(const RenderDevice&) = delete;

        RenderDevice(RenderDevice&&) = delete;
        RenderDevice& operator=(RenderDevice&&) = delete;

        auto getDriver() const noexcept { return driver; }

        virtual void start() = 0;
        virtual void process();

        auto getSampleCount() const noexcept { return sampleCount; }

        virtual std::vector<math::Size<std::uint32_t, 2>> getSupportedResolutions() const;

        void submitCommandBuffer(CommandBuffer&& commandBuffer)
        {
            std::unique_lock lock(commandQueueMutex);
            commandQueue.push(std::move(commandBuffer));
            lock.unlock();
            commandQueueCondition.notify_all();
        }

        auto getDrawCallCount() const noexcept { return drawCallCount; }

        auto getAPIMajorVersion() const noexcept { return apiVersion.v[0]; }
        auto getAPIMinorVersion() const noexcept { return apiVersion.v[1]; }

        auto isNPOTTexturesSupported() const noexcept { return npotTexturesSupported; }
        auto isAnisotropicFilteringSupported() const noexcept { return anisotropicFilteringSupported; }
        auto isRenderTargetsSupported() const noexcept { return renderTargetsSupported; }

        auto& getProjectionTransform(bool renderTarget) const noexcept
        {
            return renderTarget ? renderTargetProjectionTransform : projectionTransform;
        }

        bool hasEvents() const;
        Event getNextEvent();

        float getFPS() const noexcept { return currentFPS; }
        float getAccumulatedFPS() const noexcept { return accumulatedFPS; }

        void executeOnRenderThread(const std::function<void()>& func);

        using ResourceId = std::size_t;
        class Resource final
        {
        public:
            Resource() = default;
            Resource(RenderDevice& initRenderDevice):
                renderDevice{&initRenderDevice}, id{initRenderDevice.createResourceId()}
            {
            }

            ~Resource()
            {
                if (renderDevice && id) renderDevice->deleteResourceId(id);
            }

            Resource(const Resource&) = delete;
            Resource& operator=(const Resource&) = delete;

            Resource(Resource&& other) noexcept:
                renderDevice{other.renderDevice}, id(other.id)
            {
                other.renderDevice = nullptr;
                other.id = 0;
            }

            Resource& operator=(Resource&& other) noexcept
            {
                if (&other == this) return *this;

                if (renderDevice && id) renderDevice->deleteResourceId(id);
                renderDevice = other.renderDevice;
                id = other.id;
                other.renderDevice = nullptr;
                other.id = 0;

                return *this;
            }

            operator ResourceId() const noexcept { return id; }

        private:
            RenderDevice* renderDevice = nullptr;
            ResourceId id = 0;
        };

    protected:
        void executeAll();

        virtual void generateScreenshot(const std::string& filename);

        Driver driver;
        core::Window& window;

        ApiVersion apiVersion;

        std::uint32_t sampleCount = 1; // MSAA sample count

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

        math::Matrix<float, 4> projectionTransform = math::identityMatrix<float, 4>;
        math::Matrix<float, 4> renderTargetProjectionTransform = math::identityMatrix<float, 4>;

        std::uint32_t drawCallCount = 0;

        std::queue<CommandBuffer> commandQueue;
        std::mutex commandQueueMutex;
        std::condition_variable commandQueueCondition;

        std::queue<Event> eventQueue;
        mutable std::mutex eventQueueMutex;
        std::condition_variable eventQueueCondition;

        std::atomic<float> currentFPS{0.0F};
        std::chrono::steady_clock::time_point previousFrameTime;

        float accumulatedTime = 0.0F;
        float currentAccumulatedFPS = 0.0F;
        std::atomic<float> accumulatedFPS{0.0F};

        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;

    private:
        ResourceId createResourceId()
        {
            if (const auto i = deletedResourceIds.begin(); i != deletedResourceIds.end())
            {
                const auto resourceId = *i;
                deletedResourceIds.erase(i);
                return resourceId;
            }
            else
                return ++lastResourceId;
        }

        void deleteResourceId(ResourceId id)
        {
            deletedResourceIds.insert(id);
        }

        ResourceId lastResourceId = 0;
        std::set<ResourceId> deletedResourceIds;
    };
}

#endif // OUZEL_GRAPHICS_RENDERDEVICE_HPP
