// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

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
                 core::Window& newWindow,
                 const Size2U& newSize,
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
                               Color clearColor,
                               float clearDepth,
                               std::uint32_t clearStencil);
        void setScissorTest(bool enabled, const RectF& rectangle);
        void setViewport(const RectF& viewport);
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
        void pushDebugMarker(const std::string& name);
        void popDebugMarker();
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

        Vector2F convertScreenToNormalizedLocation(const Vector2F& position)
        {
            return Vector2F(position.v[0] / size.v[0],
                            1.0F - (position.v[1] / size.v[1]));
        }

        Vector2F convertNormalizedToScreenLocation(const Vector2F& position)
        {
            return Vector2F(position.v[0] * size.v[0],
                            (1.0F - position.v[1]) * size.v[1]);
        }

    private:
        void handleEvent(const RenderDevice::Event& event);
        void setSize(const Size2U& newSize);

        std::unique_ptr<RenderDevice> device;
        renderer::Renderer renderer;

        SamplerFilter textureFilter = SamplerFilter::point;
        std::uint32_t maxAnisotropy = 1;

        Size2U size;
        CommandBuffer commandBuffer;

        bool newFrame = false;
        std::mutex frameMutex;
        std::condition_variable frameCondition;
        std::atomic_bool refillQueue{true};
    };
}

#endif // OUZEL_GRAPHICS_GRAPHICS_HPP
