// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
#include "math/Matrix4.hpp"
#include "math/Size2.hpp"
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
                     const Size2<uint32_t>& newSize,
                     uint32_t newSampleCount,
                     Texture::Filter newTextureFilter,
                     uint32_t newMaxAnisotropy,
                     bool newVerticalSync,
                     bool newDepth,
                     bool newDebugRenderer);

            Renderer(const Renderer&) = delete;
            Renderer& operator=(const Renderer&) = delete;

            Renderer(Renderer&&) = delete;
            Renderer& operator=(Renderer&&) = delete;

            static Driver getDriver(const std::string& driver);
            static std::set<Driver> getAvailableRenderDrivers();

            inline RenderDevice* getDevice() const { return device.get(); }

            inline const Size2<uint32_t>& getSize() const { return size; }

            void saveScreenshot(const std::string& filename);

            void setRenderTarget(uintptr_t renderTarget);
            void clearRenderTarget(bool clearColorBuffer,
                                   bool clearDepthBuffer,
                                   Color clearColor,
                                   float clearDepth);
            void setCullMode(CullMode cullMode);
            void setFillMode(FillMode fillMode);
            void setScissorTest(bool enabled, const Rect<float>& rectangle);
            void setViewport(const Rect<float>& viewport);
            void setDepthStencilState(uintptr_t depthStencilState);
            void setPipelineState(uintptr_t blendState,
                                  uintptr_t shader);
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
            void present();
            void addCommand(std::unique_ptr<Command>&& command);

            void waitForNextFrame();
            inline bool getRefillQueue() const { return refillQueue; }

            Vector2<float> convertScreenToNormalizedLocation(const Vector2<float>& position)
            {
                return Vector2<float>(position.v[0] / size.v[0],
                                      1.0F - (position.v[1] / size.v[1]));
            }

            Vector2<float> convertNormalizedToScreenLocation(const Vector2<float>& position)
            {
                return Vector2<float>(position.v[0] * size.v[0],
                                      (1.0F - position.v[1]) * size.v[1]);
            }

        private:
            void handleEvent(const RenderDevice::Event& event);
            void setSize(const Size2<uint32_t>& newSize);

            std::unique_ptr<RenderDevice> device;

            Size2<uint32_t> size;
            CommandBuffer commandBuffer;

            bool newFrame = false;
            std::mutex frameMutex;
            std::condition_variable frameCondition;
            std::atomic_bool refillQueue{true};
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_RENDERER_HPP
