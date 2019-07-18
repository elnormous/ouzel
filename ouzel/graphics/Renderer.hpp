// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERER_HPP
#define OUZEL_GRAPHICS_RENDERER_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <atomic>
#include "graphics/Commands.hpp"
#include "graphics/Driver.hpp"
#include "graphics/RenderDevice.hpp"
#include "math/Rect.hpp"
#include "math/Matrix.hpp"
#include "math/Size.hpp"
#include "math/Color.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer final
        {
            friend Window;
        public:
            Renderer(Driver driver,
                     Window* newWindow,
                     const Size2U& newSize,
                     uint32_t newSampleCount,
                     SamplerFilter newTextureFilter,
                     uint32_t newMaxAnisotropy,
                     bool newSrgb,
                     bool newVerticalSync,
                     bool newDepth,
                     bool newStencil,
                     bool newDebugRenderer);

            Renderer(const Renderer&) = delete;
            Renderer& operator=(const Renderer&) = delete;

            Renderer(Renderer&&) = delete;
            Renderer& operator=(Renderer&&) = delete;

            static Driver getDriver(const std::string& driver);
            static std::set<Driver> getAvailableRenderDrivers();

            inline auto getDevice() const { return device.get(); }

            inline const auto& getSize() const noexcept { return size; }

            void saveScreenshot(const std::string& filename);

            void setRenderTarget(uintptr_t renderTarget);
            void clearRenderTarget(bool clearColorBuffer,
                                   bool clearDepthBuffer,
                                   bool clearStencilBuffer,
                                   Color clearColor,
                                   float clearDepth,
                                   uint32_t clearStencil);
            void setScissorTest(bool enabled, const RectF& rectangle);
            void setViewport(const RectF& viewport);
            void setDepthStencilState(uintptr_t depthStencilState,
                                      uint32_t stencilReferenceValue);
            void setPipelineState(uintptr_t blendState,
                                  uintptr_t shader,
                                  CullMode cullMode,
                                  FillMode fillMode);
            void draw(uintptr_t indexBuffer,
                      uint32_t indexCount,
                      uint32_t indexSize,
                      uintptr_t vertexBuffer,
                      DrawMode drawMode,
                      uint32_t startIndex);
            void pushDebugMarker(const std::string& name);
            void popDebugMarker();
            void setShaderConstants(std::vector<std::vector<float>> fragmentShaderConstants,
                                    std::vector<std::vector<float>> vertexShaderConstants);
            void setTextures(const std::vector<uintptr_t>& textures);

            inline void addCommand(std::unique_ptr<Command> command)
            {
                commandBuffer.pushCommand(std::move(command));
            }
            void present();

            void waitForNextFrame();
            inline bool getRefillQueue() const { return refillQueue; }

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

            Size2U size;
            CommandBuffer commandBuffer;

            bool newFrame = false;
            std::mutex frameMutex;
            std::condition_variable frameCondition;
            std::atomic_bool refillQueue{true};
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_RENDERER_HPP
