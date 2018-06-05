// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"
#include "Renderer.hpp"
#include "RenderDevice.hpp"
#include "core/Engine.hpp"
#include "TextureResource.hpp"
#include "ShaderResource.hpp"
#include "BlendStateResource.hpp"
#include "BufferResource.hpp"
#include "events/EventHandler.hpp"
#include "events/EventDispatcher.hpp"
#include "core/Window.hpp"
#include "utils/Log.hpp"

#if OUZEL_PLATFORM_MACOS
#include "graphics/metal/macos/RenderDeviceMetalMacOS.hpp"
#include "graphics/opengl/macos/RenderDeviceOGLMacOS.hpp"
#elif OUZEL_PLATFORM_IOS
#include "graphics/metal/ios/RenderDeviceMetalIOS.hpp"
#include "graphics/opengl/ios/RenderDeviceOGLIOS.hpp"
#elif OUZEL_PLATFORM_TVOS
#include "graphics/metal/tvos/RenderDeviceMetalTVOS.hpp"
#include "graphics/opengl/tvos/RenderDeviceOGLTVOS.hpp"
#elif OUZEL_PLATFORM_ANDROID
#include "graphics/opengl/android/RenderDeviceOGLAndroid.hpp"
#elif OUZEL_PLATFORM_LINUX
#include "graphics/opengl/linux/RenderDeviceOGLLinux.hpp"
#elif OUZEL_PLATFORM_WINDOWS
#include "graphics/opengl/windows/RenderDeviceOGLWin.hpp"
#elif OUZEL_PLATFORM_RASPBIAN
#include "graphics/opengl/raspbian/RenderDeviceOGLRasp.hpp"
#elif OUZEL_PLATFORM_EMSCRIPTEN
#include "graphics/opengl/emscripten/RenderDeviceOGLEm.hpp"
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
                {
                    availableDrivers.insert(Driver::METAL);
                }
#endif
            }

            return availableDrivers;
        }

        Renderer::Renderer(Driver driver)
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
                    Log(Log::Level::INFO) << "Using OpenGL render driver";
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
#elif OUZEL_PLATFORM_RASPBIAN
                    device.reset(new RenderDeviceOGLRasp());
#elif OUZEL_PLATFORM_EMSCRIPTEN
                    device.reset(new RenderDeviceOGLEm());
#else
                    device.reset(new RenderDeviceOGL());
#endif
                    break;
#endif
#if OUZEL_COMPILE_DIRECT3D11
                case Driver::DIRECT3D11:
                    Log(Log::Level::INFO) << "Using Direct3D 11 render driver";
                    device.reset(new RenderDeviceD3D11());
                    break;
#endif
#if OUZEL_COMPILE_METAL
                case Driver::METAL:
                    Log(Log::Level::INFO) << "Using Metal render driver";
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
                    Log(Log::Level::INFO) << "Not using render driver";
                    device.reset(new RenderDeviceEmpty());
                    break;
            }
        }

        Renderer::~Renderer()
        {
        }

        bool Renderer::init(Window* newWindow,
                            const Size2& newSize,
                            uint32_t newSampleCount,
                            Texture::Filter newTextureFilter,
                            uint32_t newMaxAnisotropy,
                            bool newVerticalSync,
                            bool newDepth,
                            bool newDebugRenderer)
        {
            size = newSize;

            if (!device->init(newWindow,
                              newSize,
                              newSampleCount,
                              newTextureFilter,
                              newMaxAnisotropy,
                              newVerticalSync,
                              newDepth,
                              newDebugRenderer))
                return false;

            std::shared_ptr<BlendState> noBlendState = std::make_shared<BlendState>();

            noBlendState->init(false,
                               BlendState::Factor::ONE, BlendState::Factor::ZERO,
                               BlendState::Operation::ADD,
                               BlendState::Factor::ONE, BlendState::Factor::ZERO,
                               BlendState::Operation::ADD);

            engine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            std::shared_ptr<BlendState> addBlendState = std::make_shared<BlendState>();

            addBlendState->init(true,
                                BlendState::Factor::ONE, BlendState::Factor::ONE,
                                BlendState::Operation::ADD,
                                BlendState::Factor::ONE, BlendState::Factor::ONE,
                                BlendState::Operation::ADD);

            engine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            std::shared_ptr<BlendState> multiplyBlendState = std::make_shared<BlendState>();

            multiplyBlendState->init(true,
                                     BlendState::Factor::DEST_COLOR, BlendState::Factor::ZERO,
                                     BlendState::Operation::ADD,
                                     BlendState::Factor::ONE, BlendState::Factor::ONE,
                                     BlendState::Operation::ADD);

            engine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

            std::shared_ptr<BlendState> alphaBlendState = std::make_shared<BlendState>();

            alphaBlendState->init(true,
                                  BlendState::Factor::SRC_ALPHA, BlendState::Factor::INV_SRC_ALPHA,
                                  BlendState::Operation::ADD,
                                  BlendState::Factor::ONE, BlendState::Factor::ONE,
                                  BlendState::Operation::ADD);

            engine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            std::shared_ptr<BlendState> screenBlendState = std::make_shared<BlendState>();

            screenBlendState->init(true,
                                   BlendState::Factor::ONE, BlendState::Factor::INV_SRC_COLOR,
                                   BlendState::Operation::ADD,
                                   BlendState::Factor::ONE, BlendState::Factor::ONE,
                                   BlendState::Operation::ADD);

            engine->getCache()->setBlendState(BLEND_SCREEN, screenBlendState);

            std::shared_ptr<Texture> whitePixelTexture = std::make_shared<Texture>();
            whitePixelTexture->init({255, 255, 255, 255}, Size2(1.0F, 1.0F), 0, 1);
            engine->getCache()->setTexture(TEXTURE_WHITE_PIXEL, whitePixelTexture);

            return true;
        }

        void Renderer::executeOnRenderThread(const std::function<void(void)>& func)
        {
            device->executeOnRenderThread(func);
        }

        void Renderer::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            executeOnRenderThread(std::bind(&RenderDevice::setClearColorBuffer, device.get(), clear));
        }

        void Renderer::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            executeOnRenderThread(std::bind(&RenderDevice::setClearDepthBuffer, device.get(), clear));
        }

        void Renderer::setClearColor(Color color)
        {
            clearColor = color;

            executeOnRenderThread(std::bind(&RenderDevice::setClearColor, device.get(), color));
        }

        void Renderer::setClearDepth(float newClearDepth)
        {
            clearDepth = newClearDepth;

            executeOnRenderThread(std::bind(&RenderDevice::setClearDepth, device.get(), newClearDepth));
        }

        void Renderer::setSize(const Size2& newSize)
        {
            size = newSize;

            executeOnRenderThread(std::bind(&RenderDevice::setSize, device.get(), size));
        }

        bool Renderer::saveScreenshot(const std::string& filename)
        {
            executeOnRenderThread(std::bind(&RenderDevice::generateScreenshot, device.get(), filename));

            return true;
        }

        bool Renderer::addSetRenderTargetCommand(const std::shared_ptr<Texture>& renderTarget)
        {
            return device->addCommand(SetRenderTargetCommand(renderTarget ? renderTarget->getResource() : nullptr));
        }

        bool Renderer::addClearCommand(const std::shared_ptr<Texture>& renderTarget)
        {
            return device->addCommand(ClearCommand(renderTarget ? renderTarget->getResource() : nullptr));
        }

        bool Renderer::addSetCullModeCommad(Renderer::CullMode cullMode)
        {
            return device->addCommand(SetCullModeCommad(cullMode));
        }

        bool Renderer::addSetFillModeCommad(Renderer::FillMode fillMode)
        {
            return device->addCommand(SetFillModeCommad(fillMode));
        }

        bool Renderer::addSetScissorTestCommand(bool enabled, const Rect& rectangle)
        {
            return device->addCommand(SetScissorTestCommand(enabled, rectangle));
        }

        bool Renderer::addSetViewportCommand(const Rect& viewport)
        {
            return device->addCommand(SetViewportCommand(viewport));
        }

        bool Renderer::addSetDepthStateCommand(bool depthTest, bool depthWrite)
        {
            return device->addCommand(SetDepthStateCommand(depthTest, depthWrite));
        }

        bool Renderer::addSetPipelineStateCommand(const std::shared_ptr<BlendState>& blendState,
                                                  const std::shared_ptr<Shader>& shader)
        {
            return device->addCommand(SetPipelineStateCommand(blendState ? blendState->getResource() : nullptr,
                                                              shader ? shader->getResource() : nullptr));
        }

        bool Renderer::addDrawCommand(const std::shared_ptr<Buffer>& indexBuffer,
                                      uint32_t indexCount,
                                      uint32_t indexSize,
                                      const std::shared_ptr<Buffer>& vertexBuffer,
                                      DrawMode drawMode,
                                      uint32_t startIndex)
        {
            if (!indexBuffer || !vertexBuffer)
            {
                Log(Log::Level::ERR) << "Invalid mesh buffer passed to render queue";
                return false;
            }

            return device->addCommand(DrawCommand(indexBuffer->getResource(),
                                                  indexCount,
                                                  indexSize,
                                                  vertexBuffer->getResource(),
                                                  drawMode,
                                                  startIndex));
        }

        bool Renderer::addPushDebugMarkerCommand(const std::string& name)
        {
            return device->addCommand(PushDebugMarkerCommand(name));
        }

        bool Renderer::addPopDebugMarkerCommand()
        {
            return device->addCommand(PopDebugMarkerCommand());
        }

        bool Renderer::addSetShaderConstantsCommand(std::vector<std::vector<float>> fragmentShaderConstants,
                                                    std::vector<std::vector<float>> vertexShaderConstants)
        {
            return device->addCommand(SetShaderConstantsCommand(fragmentShaderConstants,
                                                                vertexShaderConstants));
        }

        bool Renderer::addSetTexturesCommand(const std::vector<std::shared_ptr<Texture>>& textures)
        {
            TextureResource* newTextures[Texture::LAYERS];

            for (uint32_t i = 0; i < Texture::LAYERS; ++i)
            {
                Texture* texture = (i < textures.size()) ? textures[i].get() : nullptr;
                newTextures[i] = texture ? texture->getResource() : nullptr;
            }

            return device->addCommand(SetTexturesCommand(newTextures));
        }
    } // namespace graphics
} // namespace ouzel
