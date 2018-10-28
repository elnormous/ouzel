// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"
#include "Renderer.hpp"
#include "RenderDevice.hpp"
#include "events/EventHandler.hpp"
#include "events/EventDispatcher.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

#if OUZEL_PLATFORM_MACOS
#  include "graphics/metal/macos/RenderDeviceMetalMacOS.hpp"
#  include "graphics/opengl/macos/RenderDeviceOGLMacOS.hpp"
#elif OUZEL_PLATFORM_IOS
#  include "graphics/metal/ios/RenderDeviceMetalIOS.hpp"
#  include "graphics/opengl/ios/RenderDeviceOGLIOS.hpp"
#elif OUZEL_PLATFORM_TVOS
#  include "graphics/metal/tvos/RenderDeviceMetalTVOS.hpp"
#  include "graphics/opengl/tvos/RenderDeviceOGLTVOS.hpp"
#elif OUZEL_PLATFORM_ANDROID
#  include "graphics/opengl/android/RenderDeviceOGLAndroid.hpp"
#elif OUZEL_PLATFORM_LINUX
#  include "graphics/opengl/linux/RenderDeviceOGLLinux.hpp"
#elif OUZEL_PLATFORM_WINDOWS
#  include "graphics/opengl/windows/RenderDeviceOGLWin.hpp"
#elif OUZEL_PLATFORM_EMSCRIPTEN
#  include "graphics/opengl/emscripten/RenderDeviceOGLEm.hpp"
#endif

#include "graphics/empty/RenderDeviceEmpty.hpp"
#include "graphics/opengl/RenderDeviceOGL.hpp"
#include "graphics/direct3d11/RenderDeviceD3D11.hpp"
#include "graphics/metal/RenderDeviceMetal.hpp"

static const float GAMMA = 2.2F;
uint8_t GAMMA_ENCODE[256];
float GAMMA_DECODE[256];

namespace ouzel
{
    namespace graphics
    {
        std::set<Renderer::Driver> Renderer::getAvailableRenderDrivers()
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
                if (RenderDeviceMetal::available())
                    availableDrivers.insert(Driver::METAL);
#endif
            }

            return availableDrivers;
        }

        Renderer::Renderer(Driver driver,
                           Window* newWindow,
                           const Size2& newSize,
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
#if OUZEL_PLATFORM_MACOS
                    device.reset(new RenderDeviceOGLMacOS());
#elif OUZEL_PLATFORM_IOS
                    device.reset(new RenderDeviceOGLIOS());
#elif OUZEL_PLATFORM_TVOS
                    device.reset(new RenderDeviceOGLTVOS());
#elif OUZEL_PLATFORM_ANDROID
                    device.reset(new RenderDeviceOGLAndroid());
#elif OUZEL_PLATFORM_LINUX
                    device.reset(new RenderDeviceOGLLinux());
#elif OUZEL_PLATFORM_WINDOWS
                    device.reset(new RenderDeviceOGLWin());
#elif OUZEL_PLATFORM_EMSCRIPTEN
                    device.reset(new RenderDeviceOGLEm());
#else
                    device.reset(new RenderDeviceOGL());
#endif
                    break;
#endif
#if OUZEL_COMPILE_DIRECT3D11
                case Driver::DIRECT3D11:
                    engine->log(Log::Level::INFO) << "Using Direct3D 11 render driver";
                    device.reset(new RenderDeviceD3D11());
                    break;
#endif
#if OUZEL_COMPILE_METAL
                case Driver::METAL:
                    engine->log(Log::Level::INFO) << "Using Metal render driver";
#if OUZEL_PLATFORM_MACOS
                    device.reset(new RenderDeviceMetalMacOS());
#elif OUZEL_PLATFORM_IOS
                    device.reset(new RenderDeviceMetalIOS());
#elif OUZEL_PLATFORM_TVOS
                    device.reset(new RenderDeviceMetalTVOS());
#endif
                    break;
#endif
                default:
                    engine->log(Log::Level::INFO) << "Not using render driver";
                    device.reset(new RenderDeviceEmpty());
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

        void Renderer::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            device->addCommand(std::unique_ptr<Command>(new SetRenderTargetParametersCommand(nullptr,
                                                                                             clearColorBuffer,
                                                                                             clearDepthBuffer,
                                                                                             clearColor,
                                                                                             clearDepth)));
        }

        void Renderer::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            device->addCommand(std::unique_ptr<Command>(new SetRenderTargetParametersCommand(nullptr,
                                                                                             clearColorBuffer,
                                                                                             clearDepthBuffer,
                                                                                             clearColor,
                                                                                             clearDepth)));
        }

        void Renderer::setClearColor(Color color)
        {
            clearColor = color;

            device->addCommand(std::unique_ptr<Command>(new SetRenderTargetParametersCommand(nullptr,
                                                                                             clearColorBuffer,
                                                                                             clearDepthBuffer,
                                                                                             clearColor,
                                                                                             clearDepth)));
        }

        void Renderer::setClearDepth(float newClearDepth)
        {
            clearDepth = newClearDepth;

            device->addCommand(std::unique_ptr<Command>(new SetRenderTargetParametersCommand(nullptr,
                                                                                             clearColorBuffer,
                                                                                             clearDepthBuffer,
                                                                                             clearColor,
                                                                                             clearDepth)));
        }

        void Renderer::setSize(const Size2& newSize)
        {
            size = newSize;

            device->executeOnRenderThread(std::bind(&RenderDevice::setSize, device.get(), size));
        }

        void Renderer::saveScreenshot(const std::string& filename)
        {
            device->executeOnRenderThread(std::bind(&RenderDevice::generateScreenshot, device.get(), filename));
        }

        void Renderer::setRenderTarget(RenderResource* renderTarget)
        {
            device->addCommand(std::unique_ptr<Command>(new SetRenderTargetCommand(renderTarget)));
        }

        void Renderer::clearRenderTarget(RenderResource* renderTarget)
        {
            device->addCommand(std::unique_ptr<Command>(new ClearRenderTargetCommand(renderTarget)));
        }

        void Renderer::setCullMode(Renderer::CullMode cullMode)
        {
            device->addCommand(std::unique_ptr<Command>(new SetCullModeCommad(cullMode)));
        }

        void Renderer::setFillMode(Renderer::FillMode fillMode)
        {
            device->addCommand(std::unique_ptr<Command>(new SetFillModeCommad(fillMode)));
        }

        void Renderer::setScissorTest(bool enabled, const Rect& rectangle)
        {
            device->addCommand(std::unique_ptr<Command>(new SetScissorTestCommand(enabled, rectangle)));
        }

        void Renderer::setViewport(const Rect& viewport)
        {
            device->addCommand(std::unique_ptr<Command>(new SetViewportCommand(viewport)));
        }

        void Renderer::setDepthState(bool depthTest, bool depthWrite)
        {
            device->addCommand(std::unique_ptr<Command>(new SetDepthStateCommand(depthTest, depthWrite)));
        }

        void Renderer::setPipelineState(RenderResource* blendState,
                                        RenderResource* shader)
        {
            device->addCommand(std::unique_ptr<Command>(new SetPipelineStateCommand(blendState, shader)));
        }

        void Renderer::draw(RenderResource* indexBuffer,
                            uint32_t indexCount,
                            uint32_t indexSize,
                            RenderResource* vertexBuffer,
                            DrawMode drawMode,
                            uint32_t startIndex)
        {
            if (!indexBuffer || !vertexBuffer)
                throw DataError("Invalid mesh buffer passed to render queue");

            device->addCommand(std::unique_ptr<Command>(new DrawCommand(indexBuffer,
                                                                        indexCount,
                                                                        indexSize,
                                                                        vertexBuffer,
                                                                        drawMode,
                                                                        startIndex)));
        }

        void Renderer::pushDebugMarker(const std::string& name)
        {
            device->addCommand(std::unique_ptr<Command>(new PushDebugMarkerCommand(name)));
        }

        void Renderer::popDebugMarker()
        {
            device->addCommand(std::unique_ptr<Command>(new PopDebugMarkerCommand()));
        }

        void Renderer::setShaderConstants(std::vector<std::vector<float>> fragmentShaderConstants,
                                          std::vector<std::vector<float>> vertexShaderConstants)
        {
            device->addCommand(std::unique_ptr<Command>(new SetShaderConstantsCommand(fragmentShaderConstants,
                                                                                      vertexShaderConstants)));
        }

        void Renderer::setTextures(const std::vector<RenderResource*>& textures)
        {
            RenderResource* newTextures[Texture::LAYERS];

            for (uint32_t i = 0; i < Texture::LAYERS; ++i)
                newTextures[i] = (i < textures.size()) ? textures[i] : nullptr;

            device->addCommand(std::unique_ptr<Command>(new SetTexturesCommand(newTextures)));
        }

        void Renderer::waitForNextFrame()
        {
            device->waitForNextFrame();
        }
    } // namespace graphics
} // namespace ouzel
