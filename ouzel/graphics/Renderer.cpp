// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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

#if OUZEL_COMPILE_OPENGL
#  if OUZEL_SUPPORTS_OPENGLES
#    include "opengl/ColorPSGLES2.h"
#    include "opengl/ColorVSGLES2.h"
#    include "opengl/TexturePSGLES2.h"
#    include "opengl/TextureVSGLES2.h"
#    include "opengl/ColorPSGLES3.h"
#    include "opengl/ColorVSGLES3.h"
#    include "opengl/TexturePSGLES3.h"
#    include "opengl/TextureVSGLES3.h"
#  else
#    include "opengl/ColorPSGL2.h"
#    include "opengl/ColorVSGL2.h"
#    include "opengl/TexturePSGL2.h"
#    include "opengl/TextureVSGL2.h"
#    include "opengl/ColorPSGL3.h"
#    include "opengl/ColorVSGL3.h"
#    include "opengl/TexturePSGL3.h"
#    include "opengl/TextureVSGL3.h"
#    include "opengl/ColorPSGL4.h"
#    include "opengl/ColorVSGL4.h"
#    include "opengl/TexturePSGL4.h"
#    include "opengl/TextureVSGL4.h"
#  endif
#endif

#if OUZEL_COMPILE_DIRECT3D11
#  include "direct3d11/TexturePSD3D11.h"
#  include "direct3d11/TextureVSD3D11.h"
#  include "direct3d11/ColorPSD3D11.h"
#  include "direct3d11/ColorVSD3D11.h"
#endif

#if OUZEL_COMPILE_METAL
#  if OUZEL_PLATFORM_MACOS
#    include "metal/ColorPSMacOS.h"
#    include "metal/ColorVSMacOS.h"
#    include "metal/TexturePSMacOS.h"
#    include "metal/TextureVSMacOS.h"
#    define COLOR_PIXEL_SHADER_METAL ColorPSMacOS_metallib
#    define COLOR_VERTEX_SHADER_METAL ColorVSMacOS_metallib
#    define TEXTURE_PIXEL_SHADER_METAL TexturePSMacOS_metallib
#    define TEXTURE_VERTEX_SHADER_METAL TextureVSMacOS_metallib
#  elif OUZEL_PLATFORM_TVOS
#    include "metal/ColorPSTVOS.h"
#    include "metal/ColorVSTVOS.h"
#    include "metal/TexturePSTVOS.h"
#    include "metal/TextureVSTVOS.h"
#    define COLOR_PIXEL_SHADER_METAL ColorPSTVOS_metallib
#    define COLOR_VERTEX_SHADER_METAL ColorVSTVOS_metallib
#    define TEXTURE_PIXEL_SHADER_METAL TexturePSTVOS_metallib
#    define TEXTURE_VERTEX_SHADER_METAL TextureVSTVOS_metallib
#  elif OUZEL_PLATFORM_IOS
#    include "metal/ColorPSIOS.h"
#    include "metal/ColorVSIOS.h"
#    include "metal/TexturePSIOS.h"
#    include "metal/TextureVSIOS.h"
#    define COLOR_PIXEL_SHADER_METAL ColorPSIOS_metallib
#    define COLOR_VERTEX_SHADER_METAL ColorVSIOS_metallib
#    define TEXTURE_PIXEL_SHADER_METAL TexturePSIOS_metallib
#    define TEXTURE_VERTEX_SHADER_METAL TextureVSIOS_metallib
#  endif
#endif

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

        void Renderer::init(Window* newWindow,
                            const Size2& newSize,
                            uint32_t newSampleCount,
                            Texture::Filter newTextureFilter,
                            uint32_t newMaxAnisotropy,
                            bool newVerticalSync,
                            bool newDepth,
                            bool newDebugRenderer)
        {
            size = newSize;

            device->init(newWindow,
                         newSize,
                         newSampleCount,
                         newTextureFilter,
                         newMaxAnisotropy,
                         newVerticalSync,
                         newDepth,
                         newDebugRenderer);

            switch (device->getDriver())
            {
#if OUZEL_COMPILE_OPENGL
                case Driver::OPENGL:
                {
                    std::shared_ptr<Shader> textureShader = std::make_shared<Shader>();

                    switch (device->getAPIMajorVersion())
                    {
#  if OUZEL_SUPPORTS_OPENGLES
                        case 2:
                            textureShader->init(std::vector<uint8_t>(std::begin(TexturePSGLES2_glsl), std::end(TexturePSGLES2_glsl)),
                                                std::vector<uint8_t>(std::begin(TextureVSGLES2_glsl), std::end(TextureVSGLES2_glsl)),
                                                {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR, Vertex::Attribute::Usage::TEXTURE_COORDINATES0},
                                                {{"color", DataType::FLOAT_VECTOR4}},
                                                {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                            break;
                        case 3:
                            textureShader->init(std::vector<uint8_t>(std::begin(TexturePSGLES3_glsl), std::end(TexturePSGLES3_glsl)),
                                                std::vector<uint8_t>(std::begin(TextureVSGLES3_glsl), std::end(TextureVSGLES3_glsl)),
                                                {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR, Vertex::Attribute::Usage::TEXTURE_COORDINATES0},
                                                {{"color", DataType::FLOAT_VECTOR4}},
                                                {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                            break;
#  else
                        case 2:
                            textureShader->init(std::vector<uint8_t>(std::begin(TexturePSGL2_glsl), std::end(TexturePSGL2_glsl)),
                                                std::vector<uint8_t>(std::begin(TextureVSGL2_glsl), std::end(TextureVSGL2_glsl)),
                                                {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR, Vertex::Attribute::Usage::TEXTURE_COORDINATES0},
                                                {{"color", DataType::FLOAT_VECTOR4}},
                                                {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                            break;
                        case 3:
                            textureShader->init(std::vector<uint8_t>(std::begin(TexturePSGL3_glsl), std::end(TexturePSGL3_glsl)),
                                                std::vector<uint8_t>(std::begin(TextureVSGL3_glsl), std::end(TextureVSGL3_glsl)),
                                                {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR, Vertex::Attribute::Usage::TEXTURE_COORDINATES0},
                                                {{"color", DataType::FLOAT_VECTOR4}},
                                                {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                            break;
                        case 4:
                            textureShader->init(std::vector<uint8_t>(std::begin(TexturePSGL4_glsl), std::end(TexturePSGL4_glsl)),
                                                std::vector<uint8_t>(std::begin(TextureVSGL4_glsl), std::end(TextureVSGL4_glsl)),
                                                {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR, Vertex::Attribute::Usage::TEXTURE_COORDINATES0},
                                                {{"color", DataType::FLOAT_VECTOR4}},
                                                {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                            break;
#  endif
                        default:
                            throw SystemError("Unsupported OpenGL version");
                    }

                    engine->getCache()->setShader(SHADER_TEXTURE, textureShader);

                    std::shared_ptr<Shader> colorShader = std::make_shared<Shader>();

                    switch (device->getAPIMajorVersion())
                    {
#  if OUZEL_SUPPORTS_OPENGLES
                        case 2:
                            colorShader->init(std::vector<uint8_t>(std::begin(ColorPSGLES2_glsl), std::end(ColorPSGLES2_glsl)),
                                              std::vector<uint8_t>(std::begin(ColorVSGLES2_glsl), std::end(ColorVSGLES2_glsl)),
                                              {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR},
                                              {{"color", DataType::FLOAT_VECTOR4}},
                                              {{"modelViewProj", DataType::FLOAT_MATRIX4}});

                            break;
                        case 3:
                            colorShader->init(std::vector<uint8_t>(std::begin(ColorPSGLES3_glsl), std::end(ColorPSGLES3_glsl)),
                                              std::vector<uint8_t>(std::begin(ColorVSGLES3_glsl), std::end(ColorVSGLES3_glsl)),
                                              {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR},
                                              {{"color", DataType::FLOAT_VECTOR4}},
                                              {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                            break;
#  else
                        case 2:
                            colorShader->init(std::vector<uint8_t>(std::begin(ColorPSGL2_glsl), std::end(ColorPSGL2_glsl)),
                                              std::vector<uint8_t>(std::begin(ColorVSGL2_glsl), std::end(ColorVSGL2_glsl)),
                                              {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR},
                                              {{"color", DataType::FLOAT_VECTOR4}},
                                              {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                            break;
                        case 3:
                            colorShader->init(std::vector<uint8_t>(std::begin(ColorPSGL3_glsl), std::end(ColorPSGL3_glsl)),
                                              std::vector<uint8_t>(std::begin(ColorVSGL3_glsl), std::end(ColorVSGL3_glsl)),
                                              {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR},
                                              {{"color", DataType::FLOAT_VECTOR4}},
                                              {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                            break;
                        case 4:
                            colorShader->init(std::vector<uint8_t>(std::begin(ColorPSGL4_glsl), std::end(ColorPSGL4_glsl)),
                                              std::vector<uint8_t>(std::begin(ColorVSGL4_glsl), std::end(ColorVSGL4_glsl)),
                                              {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR},
                                              {{"color", DataType::FLOAT_VECTOR4}},
                                              {{"modelViewProj", DataType::FLOAT_MATRIX4}});
                            break;
#  endif
                        default:
                            throw SystemError("Unsupported OpenGL version");
                    }

                    engine->getCache()->setShader(SHADER_COLOR, colorShader);
                    break;
                }
#endif

#if OUZEL_COMPILE_DIRECT3D11
                case Driver::DIRECT3D11:
                {
                    std::shared_ptr<Shader> textureShader = std::make_shared<Shader>();
                    textureShader->init(std::vector<uint8_t>(std::begin(TEXTURE_PIXEL_SHADER_D3D11), std::end(TEXTURE_PIXEL_SHADER_D3D11)),
                                        std::vector<uint8_t>(std::begin(TEXTURE_VERTEX_SHADER_D3D11), std::end(TEXTURE_VERTEX_SHADER_D3D11)),
                                        {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR, Vertex::Attribute::Usage::TEXTURE_COORDINATES0},
                                        {{"color", DataType::FLOAT_VECTOR4}},
                                        {{"modelViewProj", DataType::FLOAT_MATRIX4}});

                    engine->getCache()->setShader(SHADER_TEXTURE, textureShader);

                    std::shared_ptr<Shader> colorShader = std::make_shared<Shader>();
                    colorShader->init(std::vector<uint8_t>(std::begin(COLOR_PIXEL_SHADER_D3D11), std::end(COLOR_PIXEL_SHADER_D3D11)),
                                      std::vector<uint8_t>(std::begin(COLOR_VERTEX_SHADER_D3D11), std::end(COLOR_VERTEX_SHADER_D3D11)),
                                      {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR},
                                      {{"color", DataType::FLOAT_VECTOR4}},
                                      {{"modelViewProj", DataType::FLOAT_MATRIX4}});

                    engine->getCache()->setShader(SHADER_COLOR, colorShader);
                    break;
                }
#endif

#if OUZEL_COMPILE_METAL
                case Driver::METAL:
                {
                    std::shared_ptr<Shader> textureShader = std::make_shared<Shader>();
                    textureShader->init(std::vector<uint8_t>(std::begin(TEXTURE_PIXEL_SHADER_METAL), std::end(TEXTURE_PIXEL_SHADER_METAL)),
                                        std::vector<uint8_t>(std::begin(TEXTURE_VERTEX_SHADER_METAL), std::end(TEXTURE_VERTEX_SHADER_METAL)),
                                        {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR, Vertex::Attribute::Usage::TEXTURE_COORDINATES0},
                                        {{"color", DataType::FLOAT_VECTOR4}},
                                        {{"modelViewProj", DataType::FLOAT_MATRIX4}},
                                        256, 256,
                                        "mainPS", "mainVS");

                    engine->getCache()->setShader(SHADER_TEXTURE, textureShader);

                    std::shared_ptr<Shader> colorShader = std::make_shared<Shader>();
                    colorShader->init(std::vector<uint8_t>(std::begin(COLOR_PIXEL_SHADER_METAL), std::end(COLOR_PIXEL_SHADER_METAL)),
                                      std::vector<uint8_t>(std::begin(COLOR_VERTEX_SHADER_METAL), std::end(COLOR_VERTEX_SHADER_METAL)),
                                      {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR},
                                      {{"color", DataType::FLOAT_VECTOR4}},
                                      {{"modelViewProj", DataType::FLOAT_MATRIX4}},
                                      256, 256,
                                      "mainPS", "mainVS");

                    engine->getCache()->setShader(SHADER_COLOR, colorShader);
                    break;
                }
#endif

                default:
                {
                    std::shared_ptr<Shader> textureShader = std::make_shared<Shader>();

                    textureShader->init(std::vector<uint8_t>(),
                                        std::vector<uint8_t>(),
                                        {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR, Vertex::Attribute::Usage::TEXTURE_COORDINATES0},
                                        {{"color", DataType::FLOAT_VECTOR4}},
                                        {{"modelViewProj", DataType::FLOAT_MATRIX4}});

                    engine->getCache()->setShader(SHADER_TEXTURE, textureShader);

                    std::shared_ptr<Shader> colorShader = std::make_shared<Shader>();

                    colorShader->init(std::vector<uint8_t>(),
                                      std::vector<uint8_t>(),
                                      {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR},
                                      {{"color", DataType::FLOAT_VECTOR4}},
                                      {{"modelViewProj", DataType::FLOAT_MATRIX4}});

                    engine->getCache()->setShader(SHADER_COLOR, colorShader);
                    break;
                }
            }

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

        void Renderer::saveScreenshot(const std::string& filename)
        {
            executeOnRenderThread(std::bind(&RenderDevice::generateScreenshot, device.get(), filename));
        }

        void Renderer::addSetRenderTargetCommand(const std::shared_ptr<Texture>& renderTarget)
        {
            device->addCommand(SetRenderTargetCommand(renderTarget ? renderTarget->getResource() : nullptr));
        }

        void Renderer::addClearCommand(const std::shared_ptr<Texture>& renderTarget)
        {
            device->addCommand(ClearCommand(renderTarget ? renderTarget->getResource() : nullptr));
        }

        void Renderer::addSetCullModeCommad(Renderer::CullMode cullMode)
        {
            device->addCommand(SetCullModeCommad(cullMode));
        }

        void Renderer::addSetFillModeCommad(Renderer::FillMode fillMode)
        {
            device->addCommand(SetFillModeCommad(fillMode));
        }

        void Renderer::addSetScissorTestCommand(bool enabled, const Rect& rectangle)
        {
            device->addCommand(SetScissorTestCommand(enabled, rectangle));
        }

        void Renderer::addSetViewportCommand(const Rect& viewport)
        {
            device->addCommand(SetViewportCommand(viewport));
        }

        void Renderer::addSetDepthStateCommand(bool depthTest, bool depthWrite)
        {
            device->addCommand(SetDepthStateCommand(depthTest, depthWrite));
        }

        void Renderer::addSetPipelineStateCommand(const std::shared_ptr<BlendState>& blendState,
                                                  const std::shared_ptr<Shader>& shader)
        {
            device->addCommand(SetPipelineStateCommand(blendState ? blendState->getResource() : nullptr,
                                                       shader ? shader->getResource() : nullptr));
        }

        void Renderer::addDrawCommand(const std::shared_ptr<Buffer>& indexBuffer,
                                      uint32_t indexCount,
                                      uint32_t indexSize,
                                      const std::shared_ptr<Buffer>& vertexBuffer,
                                      DrawMode drawMode,
                                      uint32_t startIndex)
        {
            if (!indexBuffer || !vertexBuffer)
                throw DataError("Invalid mesh buffer passed to render queue");

            device->addCommand(DrawCommand(indexBuffer->getResource(),
                                           indexCount,
                                           indexSize,
                                           vertexBuffer->getResource(),
                                           drawMode,
                                           startIndex));
        }

        void Renderer::addPushDebugMarkerCommand(const std::string& name)
        {
            device->addCommand(PushDebugMarkerCommand(name));
        }

        void Renderer::addPopDebugMarkerCommand()
        {
            device->addCommand(PopDebugMarkerCommand());
        }

        void Renderer::addSetShaderConstantsCommand(std::vector<std::vector<float>> fragmentShaderConstants,
                                                    std::vector<std::vector<float>> vertexShaderConstants)
        {
            device->addCommand(SetShaderConstantsCommand(fragmentShaderConstants,
                                                         vertexShaderConstants));
        }

        void Renderer::addSetTexturesCommand(const std::vector<std::shared_ptr<Texture>>& textures)
        {
            TextureResource* newTextures[Texture::LAYERS];

            for (uint32_t i = 0; i < Texture::LAYERS; ++i)
            {
                Texture* texture = (i < textures.size()) ? textures[i].get() : nullptr;
                newTextures[i] = texture ? texture->getResource() : nullptr;
            }

            device->addCommand(SetTexturesCommand(newTextures));
        }
    } // namespace graphics
} // namespace ouzel
