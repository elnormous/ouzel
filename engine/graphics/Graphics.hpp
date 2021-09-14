// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_GRAPHICS_HPP
#define OUZEL_GRAPHICS_GRAPHICS_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <atomic>
#include "Commands.hpp"
#include "Driver.hpp"
#include "RenderDevice.hpp"
#include "Settings.hpp"
#include "renderer/Renderer.hpp"
#include "../math/Rect.hpp"
#include "../math/Matrix.hpp"
#include "../math/Size.hpp"
#include "../math/Color.hpp"

namespace ouzel::graphics
{
    class Graphics final
    {
        friend core::Window;
    public:
        Graphics(Driver driver,
                 core::Window& initWindow,
                 const Settings& settings);

        static Driver getDriver(const std::string& driver);
        static std::set<Driver> getAvailableRenderDrivers();

        auto getDevice() const noexcept { return device.get(); }

        auto& getSize() const noexcept { return size; }

        auto getTextureFilter() const noexcept { return textureFilter; }
        auto getMaxAnisotropy() const noexcept { return maxAnisotropy; }

        void saveScreenshot(const std::string& filename);

        void setRenderTarget(std::size_t renderTarget);
        void clearRenderTarget(bool clearColorBuffer,
                               bool clearDepthBuffer,
                               bool clearStencilBuffer,
                               math::Color clearColor,
                               float clearDepth,
                               std::uint32_t clearStencil);
        void setScissorTest(bool enabled, const math::Rect<float>& rectangle);
        void setViewport(const math::Rect<float>& viewport);
        void setDepthStencilState(std::size_t depthStencilState,
                                  std::uint32_t stencilReferenceValue);
        void setPipelineState(std::size_t blendState,
                              std::size_t shader,
                              CullMode cullMode,
                              FillMode fillMode);
        void draw(std::size_t indexBuffer,
                  std::uint32_t indexCount,
                  std::uint32_t indexSize,
                  std::size_t vertexBuffer,
                  DrawMode drawMode,
                  std::uint32_t startIndex);
        void setShaderConstants(const std::vector<std::vector<float>>& fragmentShaderConstants,
                                const std::vector<std::vector<float>>& vertexShaderConstants);
        void setTextures(const std::vector<std::size_t>& textures);

        void addCommand(std::unique_ptr<Command> command)
        {
            commandBuffer.pushCommand(std::move(command));
        }
        void present();

        void waitForNextFrame();
        bool getRefillQueue() const noexcept { return refillQueue; }

        math::Vector<float, 2> convertScreenToNormalizedLocation(const math::Vector<float, 2>& position)
        {
            return math::Vector<float, 2>{
                position.v[0] / size.v[0],
                1.0F - (position.v[1] / size.v[1])
            };
        }

        math::Vector<float, 2> convertNormalizedToScreenLocation(const math::Vector<float, 2>& position)
        {
            return math::Vector<float, 2>{
                position.v[0] * size.v[0],
                (1.0F - position.v[1]) * size.v[1]
            };
        }

    private:
        void handleEvent(const RenderDevice::Event& event);
        void setSize(const math::Size<std::uint32_t, 2>& newSize);

        SamplerFilter textureFilter = SamplerFilter::point;
        std::uint32_t maxAnisotropy = 1;

        math::Size<std::uint32_t, 2> size;
        CommandBuffer commandBuffer;

        bool newFrame = false;
        std::mutex frameMutex;
        std::condition_variable frameCondition;
        std::atomic_bool refillQueue{true};

        std::unique_ptr<RenderDevice> device;
        renderer::Renderer renderer;
    };
}

#endif // OUZEL_GRAPHICS_GRAPHICS_HPP
