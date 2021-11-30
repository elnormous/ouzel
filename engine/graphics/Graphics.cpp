// Ouzel by Elviss Strazdins

#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif
#include <stdexcept>
#include "../core/Setup.h"
#include "Graphics.hpp"
#include "Commands.hpp"
#include "RenderDevice.hpp"
#include "../events/EventHandler.hpp"
#include "../events/EventDispatcher.hpp"
#include "../core/Engine.hpp"
#include "../core/Window.hpp"
#include "../utils/Log.hpp"

#if TARGET_OS_IOS
#  include "metal/ios/MetalRenderDeviceIOS.hpp"
#  include "opengl/ios/OGLRenderDeviceIOS.hpp"
#elif TARGET_OS_TV
#  include "metal/tvos/MetalRenderDeviceTVOS.hpp"
#  include "opengl/tvos/OGLRenderDeviceTVOS.hpp"
#elif TARGET_OS_MAC
#  include "metal/macos/MetalRenderDeviceMacOS.hpp"
#  include "opengl/macos/OGLRenderDeviceMacOS.hpp"
#elif defined(__ANDROID__)
#  include "opengl/android/OGLRenderDeviceAndroid.hpp"
#elif defined(__linux__)
#  include "opengl/linux/OGLRenderDeviceLinux.hpp"
#elif defined(_WIN32)
#  include "opengl/windows/OGLRenderDeviceWin.hpp"
#elif defined(__EMSCRIPTEN__)
#  include "opengl/emscripten/OGLRenderDeviceEm.hpp"
#endif

#include "empty/EmptyRenderDevice.hpp"
#include "opengl/OGLRenderDevice.hpp"
#include "direct3d11/D3D11RenderDevice.hpp"
#include "metal/MetalRenderDevice.hpp"

namespace ouzel::graphics
{
    std::set<Driver> getAvailableRenderDrivers()
    {
        static std::set<Driver> availableDrivers;

        if (availableDrivers.empty())
        {
            availableDrivers.insert(Driver::empty);

#if OUZEL_COMPILE_OPENGL
            availableDrivers.insert(Driver::openGl);
#endif
#if OUZEL_COMPILE_DIRECT3D11
            availableDrivers.insert(Driver::direct3D11);
#endif
#if OUZEL_COMPILE_METAL
            if (metal::RenderDevice::available())
                availableDrivers.insert(Driver::metal);
#endif
        }

        return availableDrivers;
    }

    Driver getDriver(const std::string& driver)
    {
        if (driver.empty() || driver == "default")
        {
            const auto availableDrivers = getAvailableRenderDrivers();

            if (availableDrivers.find(Driver::metal) != availableDrivers.end())
                return Driver::metal;
            else if (availableDrivers.find(Driver::direct3D11) != availableDrivers.end())
                return Driver::direct3D11;
            else if (availableDrivers.find(Driver::openGl) != availableDrivers.end())
                return Driver::openGl;
            else
                return Driver::empty;
        }
        else if (driver == "empty")
            return Driver::empty;
        else if (driver == "opengl")
            return Driver::openGl;
        else if (driver == "direct3d11")
            return Driver::direct3D11;
        else if (driver == "metal")
            return Driver::metal;
        else
            throw std::runtime_error("Invalid graphics driver");
    }

    namespace
    {
        std::unique_ptr<RenderDevice> createRenderDevice(Driver driver,
                                                         core::Window& window,
                                                         const Settings& settings)
        {
            switch (driver)
            {
#if OUZEL_COMPILE_OPENGL
                case Driver::openGl:
                    logger.log(Log::Level::info) << "Using OpenGL render driver";
#  if TARGET_OS_IOS
                    return std::make_unique<opengl::ios::RenderDevice>(settings, window);
#  elif TARGET_OS_TV
                    return std::make_unique<opengl::tvos::RenderDevice>(settings, window);
#  elif TARGET_OS_MAC
                    return std::make_unique<opengl::macos::RenderDevice>(settings, window);
#  elif defined(__ANDROID__)
                    return std::make_unique<opengl::android::RenderDevice>(settings, window);
#  elif defined(__linux__)
                    return std::make_unique<opengl::linux::RenderDevice>(settings, window);
#  elif defined(_WIN32)
                    return std::make_unique<opengl::windows::RenderDevice>(settings, window);
#  elif defined(__EMSCRIPTEN__)
                    return std::make_unique<opengl::emscripten::RenderDevice>(settings, window);
#  else
                    return std::make_unique<opengl::RenderDevice>(settings, window);
#  endif
#endif
#if OUZEL_COMPILE_DIRECT3D11
                case Driver::direct3D11:
                    logger.log(Log::Level::info) << "Using Direct3D 11 render driver";
                    return std::make_unique<d3d11::RenderDevice>(settings, window);
#endif
#if OUZEL_COMPILE_METAL
                case Driver::metal:
                    logger.log(Log::Level::info) << "Using Metal render driver";
#  if TARGET_OS_IOS
                    return std::make_unique<metal::ios::RenderDevice>(settings, window);
#  elif TARGET_OS_TV
                    return std::make_unique<metal::tvos::RenderDevice>(settings, window);
#  elif TARGET_OS_MAC
                    return std::make_unique<metal::macos::RenderDevice>(settings, window);
#  endif
#endif
                default:
                    logger.log(Log::Level::info) << "Not using render driver";
                    return std::make_unique<empty::RenderDevice>(settings, window);
            }
        }
    }

    Graphics::Graphics(Driver driver,
                       core::Window& initWindow,
                       const Settings& settings):
        textureFilter{settings.textureFilter},
        maxAnisotropy{settings.maxAnisotropy},
        size{initWindow.getResolution()},
        device{createRenderDevice(driver, initWindow, settings)},
        renderer{*device}
    {
        device->start();
    }

    void Graphics::setSize(const math::Size<std::uint32_t, 2>& newSize)
    {
        size = newSize;

        addCommand(std::make_unique<ResizeCommand>(newSize));
    }

    void Graphics::saveScreenshot(const std::string& filename)
    {
        device->executeOnRenderThread(std::bind(&RenderDevice::generateScreenshot, device.get(), filename));
    }

    void Graphics::setRenderTarget(std::size_t renderTarget)
    {
        addCommand(std::make_unique<SetRenderTargetCommand>(renderTarget));
    }

    void Graphics::clearRenderTarget(bool clearColorBuffer,
                                     bool clearDepthBuffer,
                                     bool clearStencilBuffer,
                                     math::Color clearColor,
                                     float clearDepth,
                                     std::uint32_t clearStencil)
    {
        addCommand(std::make_unique<ClearRenderTargetCommand>(clearColorBuffer,
                                                              clearDepthBuffer,
                                                              clearStencilBuffer,
                                                              clearColor,
                                                              clearDepth,
                                                              clearStencil));
    }

    void Graphics::setScissorTest(bool enabled, const math::Rect<float>& rectangle)
    {
        addCommand(std::make_unique<SetScissorTestCommand>(enabled, rectangle));
    }

    void Graphics::setViewport(const math::Rect<float>& viewport)
    {
        addCommand(std::make_unique<SetViewportCommand>(viewport));
    }

    void Graphics::setDepthStencilState(std::size_t depthStencilState,
                                        std::uint32_t stencilReferenceValue)
    {
        addCommand(std::make_unique<SetDepthStencilStateCommand>(depthStencilState,
                                                                 stencilReferenceValue));
    }

    void Graphics::setPipelineState(std::size_t blendState,
                                    std::size_t shader,
                                    CullMode cullMode,
                                    FillMode fillMode)
    {
        addCommand(std::make_unique<SetPipelineStateCommand>(blendState,
                                                             shader,
                                                             cullMode,
                                                             fillMode));
    }

    void Graphics::draw(std::size_t indexBuffer,
                        std::uint32_t indexCount,
                        std::uint32_t indexSize,
                        std::size_t vertexBuffer,
                        DrawMode drawMode,
                        std::uint32_t startIndex)
    {
        if (!indexBuffer || !vertexBuffer)
            throw std::runtime_error("Invalid mesh buffer passed to render queue");

        addCommand(std::make_unique<DrawCommand>(indexBuffer,
                                                 indexCount,
                                                 indexSize,
                                                 vertexBuffer,
                                                 drawMode,
                                                 startIndex));
    }

    void Graphics::setShaderConstants(const std::vector<std::vector<float>>& fragmentShaderConstants,
                                      const std::vector<std::vector<float>>& vertexShaderConstants)
    {
        addCommand(std::make_unique<SetShaderConstantsCommand>(fragmentShaderConstants,
                                                               vertexShaderConstants));
    }

    void Graphics::setTextures(const std::vector<std::size_t>& textures)
    {
        addCommand(std::make_unique<SetTexturesCommand>(textures));
    }

    void Graphics::present()
    {
        addCommand(std::make_unique<RenderSceneCommand>());
        addCommand(std::make_unique<PresentCommand>());
        device->submitCommandBuffer(std::move(commandBuffer));
        commandBuffer = CommandBuffer();
    }

    bool Graphics::getRefillQueue(bool waitForNextFrame) const
    {
        for (;;)
        {
            if (!waitForNextFrame && !device->hasEvents())
                return false;

            const auto event = device->getNextEvent();
            if (event.type == RenderDevice::Event::Type::frame)
                return true;
        }
    }
}
