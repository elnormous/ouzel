// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif
#include <stdexcept>
#include "core/Setup.h"
#include "Renderer.hpp"
#include "Commands.hpp"
#include "RenderDevice.hpp"
#include "events/EventHandler.hpp"
#include "events/EventDispatcher.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "utils/Log.hpp"

#if TARGET_OS_IOS
#  include "graphics/metal/ios/MetalRenderDeviceIOS.hpp"
#  include "graphics/opengl/ios/OGLRenderDeviceIOS.hpp"
#elif TARGET_OS_TV
#  include "graphics/metal/tvos/MetalRenderDeviceTVOS.hpp"
#  include "graphics/opengl/tvos/OGLRenderDeviceTVOS.hpp"
#elif TARGET_OS_MAC
#  include "graphics/metal/macos/MetalRenderDeviceMacOS.hpp"
#  include "graphics/opengl/macos/OGLRenderDeviceMacOS.hpp"
#elif defined(__ANDROID__)
#  include "graphics/opengl/android/OGLRenderDeviceAndroid.hpp"
#elif defined(__linux__)
#  include "graphics/opengl/linux/OGLRenderDeviceLinux.hpp"
#elif defined(_WIN32)
#  include "graphics/opengl/windows/OGLRenderDeviceWin.hpp"
#elif defined(__EMSCRIPTEN__)
#  include "graphics/opengl/emscripten/OGLRenderDeviceEm.hpp"
#endif

#include "graphics/empty/EmptyRenderDevice.hpp"
#include "graphics/opengl/OGLRenderDevice.hpp"
#include "graphics/direct3d11/D3D11RenderDevice.hpp"
#include "graphics/metal/MetalRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        Driver Renderer::getDriver(const std::string& driver)
        {
            if (driver.empty() || driver == "default")
            {
                auto availableDrivers = getAvailableRenderDrivers();

                if (availableDrivers.find(Driver::METAL) != availableDrivers.end())
                    return Driver::METAL;
                else if (availableDrivers.find(Driver::DIRECT3D11) != availableDrivers.end())
                    return Driver::DIRECT3D11;
                else if (availableDrivers.find(Driver::OPENGL) != availableDrivers.end())
                    return Driver::OPENGL;
                else
                    return Driver::EMPTY;
            }
            else if (driver == "empty")
                return Driver::EMPTY;
            else if (driver == "opengl")
                return Driver::OPENGL;
            else if (driver == "direct3d11")
                return Driver::DIRECT3D11;
            else if (driver == "metal")
                return Driver::METAL;
            else
                throw std::runtime_error("Invalid graphics driver");
        }

        std::set<Driver> Renderer::getAvailableRenderDrivers()
        {
            static std::set<Driver> availableDrivers;

            if (availableDrivers.empty())
            {
                availableDrivers.insert(Driver::EMPTY);

#if OUZEL_COMPILE_OPENGL
                availableDrivers.insert(Driver::OPENGL);
#endif
#if OUZEL_COMPILE_DIRECT3D11
                availableDrivers.insert(Driver::DIRECT3D11);
#endif
#if OUZEL_COMPILE_METAL
                if (metal::RenderDevice::available())
                    availableDrivers.insert(Driver::METAL);
#endif
            }

            return availableDrivers;
        }

        Renderer::Renderer(Driver driver,
                           Window* newWindow,
                           const Size2U& newSize,
                           uint32_t newSampleCount,
                           SamplerFilter newTextureFilter,
                           uint32_t newMaxAnisotropy,
                           bool newSrgb,
                           bool newVerticalSync,
                           bool newDepth,
                           bool newStencil,
                           bool newDebugRenderer)
        {
            switch (driver)
            {
#if OUZEL_COMPILE_OPENGL
                case Driver::OPENGL:
                    engine->log(Log::Level::INFO) << "Using OpenGL render driver";
#  if TARGET_OS_IOS
                    device.reset(new opengl::RenderDeviceIOS(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif TARGET_OS_TV
                    device.reset(new opengl::RenderDeviceTVOS(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif TARGET_OS_MAC
                    device.reset(new opengl::RenderDeviceMacOS(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif defined(__ANDROID__)
                    device.reset(new opengl::RenderDeviceAndroid(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif defined(__linux__)
                    device.reset(new opengl::RenderDeviceLinux(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif defined(_WIN32)
                    device.reset(new opengl::RenderDeviceWin(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif defined(__EMSCRIPTEN__)
                    device.reset(new opengl::RenderDeviceEm(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  else
                    device.reset(new opengl::RenderDevice(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  endif
                    break;
#endif
#if OUZEL_COMPILE_DIRECT3D11
                case Driver::DIRECT3D11:
                    engine->log(Log::Level::INFO) << "Using Direct3D 11 render driver";
                    device.reset(new d3d11::RenderDevice(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
                    break;
#endif
#if OUZEL_COMPILE_METAL
                case Driver::METAL:
                    engine->log(Log::Level::INFO) << "Using Metal render driver";
#  if TARGET_OS_IOS
                    device.reset(new metal::RenderDeviceIOS(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif TARGET_OS_TV
                    device.reset(new metal::RenderDeviceTVOS(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif TARGET_OS_MAC
                    device.reset(new metal::RenderDeviceMacOS(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  endif
                    break;
#endif
                default:
                    engine->log(Log::Level::INFO) << "Not using render driver";
                    device.reset(new empty::RenderDevice(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
                    break;
            }

            size = newSize;

            device->init(newWindow,
                         newSize,
                         newSampleCount,
                         newTextureFilter,
                         newMaxAnisotropy,
                         newSrgb,
                         newVerticalSync,
                         newDepth,
                         newStencil,
                         newDebugRenderer);
        }

        void Renderer::handleEvent(const RenderDevice::Event& event)
        {
            if (event.type == RenderDevice::Event::Type::FRAME)
            {
                std::unique_lock<std::mutex> lock(frameMutex);
                newFrame = true;
                refillQueue = true;
                lock.unlock();
                frameCondition.notify_all();
            }
        }

        void Renderer::setSize(const Size2U& newSize)
        {
            size = newSize;

            addCommand(std::unique_ptr<Command>(new ResizeCommand(newSize)));
        }

        void Renderer::saveScreenshot(const std::string& filename)
        {
            device->executeOnRenderThread(std::bind(&RenderDevice::generateScreenshot, device.get(), filename));
        }

        void Renderer::setRenderTarget(uintptr_t renderTarget)
        {
            addCommand(std::unique_ptr<Command>(new SetRenderTargetCommand(renderTarget)));
        }

        void Renderer::clearRenderTarget(bool clearColorBuffer,
                                         bool clearDepthBuffer,
                                         bool clearStencilBuffer,
                                         Color clearColor,
                                         float clearDepth,
                                         uint32_t clearStencil)
        {
            addCommand(std::unique_ptr<Command>(new ClearRenderTargetCommand(clearColorBuffer,
                                                                             clearDepthBuffer,
                                                                             clearStencilBuffer,
                                                                             clearColor,
                                                                             clearDepth,
                                                                             clearStencil)));
        }

        void Renderer::setScissorTest(bool enabled, const RectF& rectangle)
        {
            addCommand(std::unique_ptr<Command>(new SetScissorTestCommand(enabled, rectangle)));
        }

        void Renderer::setViewport(const RectF& viewport)
        {
            addCommand(std::unique_ptr<Command>(new SetViewportCommand(viewport)));
        }

        void Renderer::setDepthStencilState(uintptr_t depthStencilState,
                                            uint32_t stencilReferenceValue)
        {
            addCommand(std::unique_ptr<Command>(new SetDepthStencilStateCommand(depthStencilState,
                                                                                stencilReferenceValue)));
        }

        void Renderer::setPipelineState(uintptr_t blendState,
                                        uintptr_t shader,
                                        CullMode cullMode,
                                        FillMode fillMode)
        {
            addCommand(std::unique_ptr<Command>(new SetPipelineStateCommand(blendState,
                                                                            shader,
                                                                            cullMode,
                                                                            fillMode)));
        }

        void Renderer::draw(uintptr_t indexBuffer,
                            uint32_t indexCount,
                            uint32_t indexSize,
                            uintptr_t vertexBuffer,
                            DrawMode drawMode,
                            uint32_t startIndex)
        {
            if (!indexBuffer || !vertexBuffer)
                throw std::runtime_error("Invalid mesh buffer passed to render queue");

            addCommand(std::unique_ptr<Command>(new DrawCommand(indexBuffer,
                                                                indexCount,
                                                                indexSize,
                                                                vertexBuffer,
                                                                drawMode,
                                                                startIndex)));
        }

        void Renderer::pushDebugMarker(const std::string& name)
        {
            addCommand(std::unique_ptr<Command>(new PushDebugMarkerCommand(name)));
        }

        void Renderer::popDebugMarker()
        {
            addCommand(std::unique_ptr<Command>(new PopDebugMarkerCommand()));
        }

        void Renderer::setShaderConstants(std::vector<std::vector<float>> fragmentShaderConstants,
                                          std::vector<std::vector<float>> vertexShaderConstants)
        {
            addCommand(std::unique_ptr<Command>(new SetShaderConstantsCommand(fragmentShaderConstants,
                                                                              vertexShaderConstants)));
        }

        void Renderer::setTextures(const std::vector<uintptr_t>& textures)
        {
            addCommand(std::unique_ptr<Command>(new SetTexturesCommand(textures)));
        }

        void Renderer::present()
        {
            refillQueue = false;
            addCommand(std::unique_ptr<Command>(new PresentCommand()));
            device->submitCommandBuffer(std::move(commandBuffer));
            commandBuffer = CommandBuffer();
        }

        void Renderer::waitForNextFrame()
        {
            std::unique_lock<std::mutex> lock(frameMutex);
            while (!newFrame) frameCondition.wait(lock);
            newFrame = false;
        }
    } // namespace graphics
} // namespace ouzel
