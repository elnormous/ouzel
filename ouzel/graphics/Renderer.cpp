// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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

static constexpr float GAMMA = 2.2F;
uint8_t GAMMA_ENCODE[256];
float GAMMA_DECODE[256];

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
                if (MetalRenderDevice::available())
                    availableDrivers.insert(Driver::METAL);
#endif
            }

            return availableDrivers;
        }

        Renderer::Renderer(Driver driver,
                           Window* newWindow,
                           const Size2<uint32_t>& newSize,
                           uint32_t newSampleCount,
                           Texture::Filter newTextureFilter,
                           uint32_t newMaxAnisotropy,
                           bool newVerticalSync,
                           bool newDepth,
                           bool newDebugRenderer)
        {
            for (uint32_t i = 0; i < 256; ++i)
            {
                GAMMA_ENCODE[i] = static_cast<uint8_t>(roundf(powf(i / 255.0F, 1.0F / GAMMA) * 255.0F));
                GAMMA_DECODE[i] = roundf(powf(i / 255.0F, GAMMA) * 255.0F);
            }

            switch (driver)
            {
#if OUZEL_COMPILE_OPENGL
                case Driver::OPENGL:
                    engine->log(Log::Level::INFO) << "Using OpenGL render driver";
#  if TARGET_OS_IOS
                    device.reset(new OGLRenderDeviceIOS(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif TARGET_OS_TV
                    device.reset(new OGLRenderDeviceTVOS(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif TARGET_OS_MAC
                    device.reset(new OGLRenderDeviceMacOS(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif defined(__ANDROID__)
                    device.reset(new OGLRenderDeviceAndroid(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif defined(__linux__)
                    device.reset(new OGLRenderDeviceLinux(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif defined(_WIN32)
                    device.reset(new OGLRenderDeviceWin(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif defined(__EMSCRIPTEN__)
                    device.reset(new OGLRenderDeviceEm(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  else
                    device.reset(new OGLRenderDevice(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  endif
                    break;
#endif
#if OUZEL_COMPILE_DIRECT3D11
                case Driver::DIRECT3D11:
                    engine->log(Log::Level::INFO) << "Using Direct3D 11 render driver";
                    device.reset(new D3D11RenderDevice(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
                    break;
#endif
#if OUZEL_COMPILE_METAL
                case Driver::METAL:
                    engine->log(Log::Level::INFO) << "Using Metal render driver";
#  if TARGET_OS_IOS
                    device.reset(new MetalRenderDeviceIOS(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif TARGET_OS_TV
                    device.reset(new MetalRenderDeviceTVOS(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  elif TARGET_OS_MAC
                    device.reset(new MetalRenderDeviceMacOS(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
#  endif
                    break;
#endif
                default:
                    engine->log(Log::Level::INFO) << "Not using render driver";
                    device.reset(new EmptyRenderDevice(std::bind(&Renderer::handleEvent, this, std::placeholders::_1)));
                    break;
            }

            size = newSize;

            device->init(newWindow,
                         newSize,
                         newSampleCount,
                         newTextureFilter,
                         newMaxAnisotropy,
                         newVerticalSync,
                         newDepth,
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

        void Renderer::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            addCommand(std::unique_ptr<Command>(new SetRenderTargetParametersCommand(0,
                                                                                     clearColorBuffer,
                                                                                     clearDepthBuffer,
                                                                                     clearColor,
                                                                                     clearDepth)));
        }

        void Renderer::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            addCommand(std::unique_ptr<Command>(new SetRenderTargetParametersCommand(0,
                                                                                     clearColorBuffer,
                                                                                     clearDepthBuffer,
                                                                                     clearColor,
                                                                                     clearDepth)));
        }

        void Renderer::setClearColor(Color color)
        {
            clearColor = color;

            addCommand(std::unique_ptr<Command>(new SetRenderTargetParametersCommand(0,
                                                                                     clearColorBuffer,
                                                                                     clearDepthBuffer,
                                                                                     clearColor,
                                                                                     clearDepth)));
        }

        void Renderer::setClearDepth(float newClearDepth)
        {
            clearDepth = newClearDepth;

            addCommand(std::unique_ptr<Command>(new SetRenderTargetParametersCommand(0,
                                                                                     clearColorBuffer,
                                                                                     clearDepthBuffer,
                                                                                     clearColor,
                                                                                     clearDepth)));
        }

        void Renderer::setSize(const Size2<uint32_t>& newSize)
        {
            size = newSize;

            device->executeOnRenderThread(std::bind(&RenderDevice::setSize, device.get(), size));
        }

        void Renderer::saveScreenshot(const std::string& filename)
        {
            device->executeOnRenderThread(std::bind(&RenderDevice::generateScreenshot, device.get(), filename));
        }

        void Renderer::setRenderTarget(uintptr_t renderTarget)
        {
            addCommand(std::unique_ptr<Command>(new SetRenderTargetCommand(renderTarget)));
        }

        void Renderer::clearRenderTarget(uintptr_t renderTarget)
        {
            addCommand(std::unique_ptr<Command>(new ClearRenderTargetCommand(renderTarget)));
        }

        void Renderer::setCullMode(CullMode cullMode)
        {
            addCommand(std::unique_ptr<Command>(new SetCullModeCommad(cullMode)));
        }

        void Renderer::setFillMode(FillMode fillMode)
        {
            addCommand(std::unique_ptr<Command>(new SetFillModeCommad(fillMode)));
        }

        void Renderer::setScissorTest(bool enabled, const Rect<float>& rectangle)
        {
            addCommand(std::unique_ptr<Command>(new SetScissorTestCommand(enabled, rectangle)));
        }

        void Renderer::setViewport(const Rect<float>& viewport)
        {
            addCommand(std::unique_ptr<Command>(new SetViewportCommand(viewport)));
        }

        void Renderer::setDepthStencilState(uintptr_t depthStencilState)
        {
            addCommand(std::unique_ptr<Command>(new SetDepthStencilStateCommand(depthStencilState)));
        }

        void Renderer::setPipelineState(uintptr_t blendState,
                                        uintptr_t shader)
        {
            addCommand(std::unique_ptr<Command>(new SetPipelineStateCommand(blendState, shader)));
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
            uintptr_t newTextures[Texture::LAYERS];

            for (uint32_t i = 0; i < Texture::LAYERS; ++i)
                newTextures[i] = (i < textures.size()) ? textures[i] : 0;

            addCommand(std::unique_ptr<Command>(new SetTexturesCommand(newTextures)));
        }

        void Renderer::present()
        {
            refillQueue = false;
            addCommand(std::unique_ptr<Command>(new PresentCommand()));
            device->submitCommandBuffer(std::move(commandBuffer));
            commandBuffer = CommandBuffer();
        }

        void Renderer::addCommand(std::unique_ptr<Command>&& command)
        {
            commandBuffer.commands.push(std::forward<std::unique_ptr<Command>>(command));
        }

        void Renderer::waitForNextFrame()
        {
            std::unique_lock<std::mutex> lock(frameMutex);
            while (!newFrame) frameCondition.wait(lock);
            newFrame = false;
        }
    } // namespace graphics
} // namespace ouzel
