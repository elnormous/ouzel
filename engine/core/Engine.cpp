// Ouzel by Elviss Strazdins

#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif
#include <algorithm>
#include <stdexcept>
#include "Setup.h"
#include "Engine.hpp"
#include "../utils/Utils.hpp"
#include "../graphics/Graphics.hpp"
#include "../audio/Audio.hpp"

#if OUZEL_COMPILE_OPENGL
#  include "../graphics/opengl/OGL.h"
#  if OUZEL_OPENGLES
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
#  if TARGET_OS_IOS
#    include "metal/ColorPSIOS.h"
#    include "metal/ColorVSIOS.h"
#    include "metal/TexturePSIOS.h"
#    include "metal/TextureVSIOS.h"
#    define COLOR_PIXEL_SHADER_METAL ColorPSIOS_metallib
#    define COLOR_VERTEX_SHADER_METAL ColorVSIOS_metallib
#    define TEXTURE_PIXEL_SHADER_METAL TexturePSIOS_metallib
#    define TEXTURE_VERTEX_SHADER_METAL TextureVSIOS_metallib
#  elif TARGET_OS_TV
#    include "metal/ColorPSTVOS.h"
#    include "metal/ColorVSTVOS.h"
#    include "metal/TexturePSTVOS.h"
#    include "metal/TextureVSTVOS.h"
#    define COLOR_PIXEL_SHADER_METAL ColorPSTVOS_metallib
#    define COLOR_VERTEX_SHADER_METAL ColorVSTVOS_metallib
#    define TEXTURE_PIXEL_SHADER_METAL TexturePSTVOS_metallib
#    define TEXTURE_VERTEX_SHADER_METAL TextureVSTVOS_metallib
#  elif TARGET_OS_MAC
#    include "metal/ColorPSMacOS.h"
#    include "metal/ColorVSMacOS.h"
#    include "metal/TexturePSMacOS.h"
#    include "metal/TextureVSMacOS.h"
#    define COLOR_PIXEL_SHADER_METAL ColorPSMacOS_metallib
#    define COLOR_VERTEX_SHADER_METAL ColorVSMacOS_metallib
#    define TEXTURE_PIXEL_SHADER_METAL TexturePSMacOS_metallib
#    define TEXTURE_VERTEX_SHADER_METAL TextureVSMacOS_metallib
#  endif
#endif

namespace ouzel::core
{
    std::mt19937 randomEngine(std::random_device{}());

    namespace
    {
        struct Settings final
        {
            math::Size<std::uint32_t, 2> size{};
            graphics::Driver graphicsDriver;
            graphics::Settings graphicsSettings;
            bool resizable = false;
            bool fullscreen = false;
            bool exclusiveFullscreen = false;
            bool highDpi = true; // should high DPI resolution be used
            audio::Driver audioDriver;
            audio::Settings audioSettings;
        };

        Settings parseSettings(const ini::Data& defaultSettings,
                               const ini::Data& userSettings)
        {
            Settings settings;

            const ini::Section& userEngineSection = userSettings.hasSection("engine") ? userSettings["engine"] : ini::Section{};
            const ini::Section& defaultEngineSection = defaultSettings.hasSection("engine") ? defaultSettings["engine"] : ini::Section{};

            const auto& graphicsDriverValue = userEngineSection.getValue("graphicsDriver", defaultEngineSection.getValue("graphicsDriver"));
            settings.graphicsDriver = graphics::getDriver(graphicsDriverValue);

            const auto& widthValue = userEngineSection.getValue("width", defaultEngineSection.getValue("width"));
            if (!widthValue.empty()) settings.size.v[0] = static_cast<std::uint32_t>(std::stoul(widthValue));

            const auto& heightValue = userEngineSection.getValue("height", defaultEngineSection.getValue("height"));
            if (!heightValue.empty()) settings.size.v[1] = static_cast<std::uint32_t>(std::stoul(heightValue));

            const auto& sampleCountValue = userEngineSection.getValue("sampleCount", defaultEngineSection.getValue("sampleCount"));
            if (!sampleCountValue.empty()) settings.graphicsSettings.sampleCount = static_cast<std::uint32_t>(std::stoul(sampleCountValue));

            const auto& textureFilterValue = userEngineSection.getValue("textureFilter", defaultEngineSection.getValue("textureFilter"));
            if (!textureFilterValue.empty())
            {
                if (textureFilterValue == "point")
                    settings.graphicsSettings.textureFilter = graphics::SamplerFilter::point;
                else if (textureFilterValue == "linear")
                    settings.graphicsSettings.textureFilter = graphics::SamplerFilter::linear;
                else if (textureFilterValue == "bilinear")
                    settings.graphicsSettings.textureFilter = graphics::SamplerFilter::bilinear;
                else if (textureFilterValue == "trilinear")
                    settings.graphicsSettings.textureFilter = graphics::SamplerFilter::trilinear;
                else
                    throw std::runtime_error("Invalid texture filter specified");
            }

            const auto& maxAnisotropyValue = userEngineSection.getValue("maxAnisotropy", defaultEngineSection.getValue("maxAnisotropy"));
            if (!maxAnisotropyValue.empty()) settings.graphicsSettings.maxAnisotropy = static_cast<std::uint32_t>(std::stoul(maxAnisotropyValue));

            const auto& resizableValue = userEngineSection.getValue("resizable", defaultEngineSection.getValue("resizable"));
            if (!resizableValue.empty()) settings.resizable = (resizableValue == "true" || resizableValue == "1" || resizableValue == "yes");

            const auto& fullscreenValue = userEngineSection.getValue("fullscreen", defaultEngineSection.getValue("fullscreen"));
            if (!fullscreenValue.empty()) settings.fullscreen = (fullscreenValue == "true" || fullscreenValue == "1" || fullscreenValue == "yes");

            const auto& verticalSyncValue = userEngineSection.getValue("verticalSync", defaultEngineSection.getValue("verticalSync"));
            if (!verticalSyncValue.empty()) settings.graphicsSettings.verticalSync = (verticalSyncValue == "true" || verticalSyncValue == "1" || verticalSyncValue == "yes");

            const auto& exclusiveFullscreenValue = userEngineSection.getValue("exclusiveFullscreen", defaultEngineSection.getValue("exclusiveFullscreen"));
            if (!exclusiveFullscreenValue.empty()) settings.exclusiveFullscreen = (exclusiveFullscreenValue == "true" || exclusiveFullscreenValue == "1" || exclusiveFullscreenValue == "yes");

            const auto& depthValue = userEngineSection.getValue("depth", defaultEngineSection.getValue("depth"));
            if (!depthValue.empty()) settings.graphicsSettings.depth = (depthValue == "true" || depthValue == "1" || depthValue == "yes");

            const auto& stencilValue = userEngineSection.getValue("stencil", defaultEngineSection.getValue("stencil"));
            if (!stencilValue.empty()) settings.graphicsSettings.stencil = (depthValue == "true" || depthValue == "1" || depthValue == "yes");

            const auto& debugRendererValue = userEngineSection.getValue("debugRenderer", defaultEngineSection.getValue("debugRenderer"));
            if (!debugRendererValue.empty()) settings.graphicsSettings.debugRenderer = (debugRendererValue == "true" || debugRendererValue == "1" || debugRendererValue == "yes");

            const auto& highDpiValue = userEngineSection.getValue("highDpi", defaultEngineSection.getValue("highDpi"));
            if (!highDpiValue.empty()) settings.highDpi = (highDpiValue == "true" || highDpiValue == "1" || highDpiValue == "yes");

            const auto& audioDriverValue = userEngineSection.getValue("audioDriver", defaultEngineSection.getValue("audioDriver"));
            settings.audioDriver = audio::getDriver(audioDriverValue);

            const auto& debugAudioValue = userEngineSection.getValue("debugAudio", defaultEngineSection.getValue("debugAudio"));
            if (!debugAudioValue.empty()) settings.audioSettings.debugAudio = (debugAudioValue == "true" || debugAudioValue == "1" || debugAudioValue == "yes");

            settings.audioSettings.audioDevice = userEngineSection.getValue("audioDevice", defaultEngineSection.getValue("audioDevice"));

            return settings;
        }
    }

    Engine::Engine(const std::vector<std::string>& initArgs):
        fileSystem(*this),
        assetBundle(cache, fileSystem),
        args{initArgs}
    {
        engine = this;
    }

    Engine::~Engine()
    {
        if (active)
        {
            auto event = std::make_unique<SystemEvent>();
            event->type = Event::Type::engineStop;
            eventDispatcher.postEvent(std::move(event));
        }

        paused = true;
        active = false;

#ifndef __EMSCRIPTEN__
        if (updateThread.isJoinable())
        {
            std::unique_lock lock{updateMutex};
            updateCondition.notify_all();
            lock.unlock();
            updateThread.join();
        }
#endif
    }

    void Engine::init()
    {
        thread::setCurrentThreadName("Main");

        const auto settingsPath = fileSystem.getStorageDirectory() / "settings.ini";
        const auto settings = parseSettings(fileSystem.resourceFileExists("settings.ini") ? ini::parse(fileSystem.readFile("settings.ini")) : ini::Data{},
                                            fileSystem.fileExists(settingsPath) ? ini::parse(fileSystem.readFile(settingsPath)) : ini::Data{});

        const Window::Flags windowFlags =
            (settings.resizable ? Window::Flags::resizable : Window::Flags::none) |
            (settings.fullscreen ? Window::Flags::fullscreen : Window::Flags::none) |
            (settings.exclusiveFullscreen ? Window::Flags::exclusiveFullscreen : Window::Flags::none) |
            (settings.highDpi ? Window::Flags::highDpi : Window::Flags::none);

        window = std::make_unique<Window>(*this,
                                          settings.size,
                                          windowFlags,
                                          OUZEL_APPLICATION_NAME,
                                          settings.graphicsDriver);

        graphics = std::make_unique<graphics::Graphics>(settings.graphicsDriver,
                                                        *window,
                                                        settings.graphicsSettings);

        audio = std::make_unique<audio::Audio>(settings.audioDriver, settings.audioSettings);

        inputManager = std::make_unique<input::InputManager>();

        // default assets
        switch (settings.graphicsDriver)
        {
#if OUZEL_COMPILE_OPENGL
            case graphics::Driver::openGl:
            {
                std::unique_ptr<graphics::Shader> textureShader;

                switch (graphics->getDevice()->getAPIMajorVersion())
                {
#  if OUZEL_OPENGLES
                    case 2:
                        textureShader = std::make_unique<graphics::Shader>(*graphics,
                                                                           std::vector<std::uint8_t>(std::begin(TexturePSGLES2_glsl),
                                                                                                     std::end(TexturePSGLES2_glsl)),
                                                                           std::vector<std::uint8_t>(std::begin(TextureVSGLES2_glsl),
                                                                                                     std::end(TextureVSGLES2_glsl)),
                                                                           std::set<graphics::Vertex::Attribute::Semantic>{
                                                                               {graphics::Vertex::Attribute::Usage::position},
                                                                               {graphics::Vertex::Attribute::Usage::color},
                                                                               {graphics::Vertex::Attribute::Usage::textureCoordinates, 0U}
                                                                           },
                                                                           std::vector<std::pair<std::string, graphics::DataType>>{
                                                                               {"color",graphics::DataType::float32Vector4}
                                                                           },
                                                                           std::vector<std::pair<std::string, graphics::DataType>>{
                                                                               {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                           });
                        break;
                    case 3:
                        textureShader = std::make_unique<graphics::Shader>(*graphics,
                                                                           std::vector<std::uint8_t>(std::begin(TexturePSGLES3_glsl),
                                                                                                     std::end(TexturePSGLES3_glsl)),
                                                                           std::vector<std::uint8_t>(std::begin(TextureVSGLES3_glsl),
                                                                                                     std::end(TextureVSGLES3_glsl)),
                                                                           std::set<graphics::Vertex::Attribute::Semantic>{
                                                                               {graphics::Vertex::Attribute::Usage::position},
                                                                               {graphics::Vertex::Attribute::Usage::color},
                                                                               {graphics::Vertex::Attribute::Usage::textureCoordinates, 0U}
                                                                           },
                                                                           std::vector<std::pair<std::string, graphics::DataType>>{
                                                                               {"color", graphics::DataType::float32Vector4}
                                                                           },
                                                                           std::vector<std::pair<std::string, graphics::DataType>>{
                                                                               {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                           });
                        break;
#  else
                    case 2:
                        textureShader = std::make_unique<graphics::Shader>(*graphics,
                                                                           std::vector<std::uint8_t>(std::begin(TexturePSGL2_glsl),
                                                                                                     std::end(TexturePSGL2_glsl)),
                                                                           std::vector<std::uint8_t>(std::begin(TextureVSGL2_glsl),
                                                                                                     std::end(TextureVSGL2_glsl)),
                                                                           std::set<graphics::Vertex::Attribute::Semantic>{
                                                                               {graphics::Vertex::Attribute::Usage::position},
                                                                               {graphics::Vertex::Attribute::Usage::color},
                                                                               {graphics::Vertex::Attribute::Usage::textureCoordinates, 0U}
                                                                           },
                                                                           std::vector<std::pair<std::string, graphics::DataType>>{
                                                                               {"color", graphics::DataType::float32Vector4}
                                                                           },
                                                                           std::vector<std::pair<std::string, graphics::DataType>>{
                                                                               {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                           });
                        break;
                    case 3:
                        textureShader = std::make_unique<graphics::Shader>(*graphics,
                                                                           std::vector<std::uint8_t>(std::begin(TexturePSGL3_glsl),
                                                                                                     std::end(TexturePSGL3_glsl)),
                                                                           std::vector<std::uint8_t>(std::begin(TextureVSGL3_glsl),
                                                                                                     std::end(TextureVSGL3_glsl)),
                                                                           std::set<graphics::Vertex::Attribute::Semantic>{
                                                                               {graphics::Vertex::Attribute::Usage::position},
                                                                               {graphics::Vertex::Attribute::Usage::color},
                                                                               {graphics::Vertex::Attribute::Usage::textureCoordinates, 0U}
                                                                           },
                                                                           std::vector<std::pair<std::string, graphics::DataType>>{
                                                                               {"color", graphics::DataType::float32Vector4}
                                                                           },
                                                                           std::vector<std::pair<std::string, graphics::DataType>>{
                                                                               {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                           });
                        break;
                    case 4:
                        textureShader = std::make_unique<graphics::Shader>(*graphics,
                                                                           std::vector<std::uint8_t>(std::begin(TexturePSGL4_glsl),
                                                                                                     std::end(TexturePSGL4_glsl)),
                                                                           std::vector<std::uint8_t>(std::begin(TextureVSGL4_glsl),
                                                                                                     std::end(TextureVSGL4_glsl)),
                                                                           std::set<graphics::Vertex::Attribute::Semantic>{
                                                                               {graphics::Vertex::Attribute::Usage::position},
                                                                               {graphics::Vertex::Attribute::Usage::color},
                                                                               {graphics::Vertex::Attribute::Usage::textureCoordinates, 0U}
                                                                           },
                                                                           std::vector<std::pair<std::string, graphics::DataType>>{
                                                                               {"color", graphics::DataType::float32Vector4}
                                                                           },
                                                                           std::vector<std::pair<std::string, graphics::DataType>>{
                                                                               {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                           });
                        break;
#  endif
                    default:
                        throw std::runtime_error("Unsupported OpenGL version");
                }

                assetBundle.setShader(shaderTexture, std::move(textureShader));

                std::unique_ptr<graphics::Shader> colorShader;

                switch (graphics->getDevice()->getAPIMajorVersion())
                {
#  if OUZEL_OPENGLES
                    case 2:
                        colorShader = std::make_unique<graphics::Shader>(*graphics,
                                                                         std::vector<std::uint8_t>(std::begin(ColorPSGLES2_glsl),
                                                                                                   std::end(ColorPSGLES2_glsl)),
                                                                         std::vector<std::uint8_t>(std::begin(ColorVSGLES2_glsl),
                                                                                                   std::end(ColorVSGLES2_glsl)),
                                                                         std::set<graphics::Vertex::Attribute::Semantic>{
                                                                             {graphics::Vertex::Attribute::Usage::position},
                                                                             {graphics::Vertex::Attribute::Usage::color}
                                                                         },
                                                                         std::vector<std::pair<std::string, graphics::DataType>>{
                                                                             {"color", graphics::DataType::float32Vector4}
                                                                         },
                                                                         std::vector<std::pair<std::string, graphics::DataType>>{
                                                                             {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                         });
                        break;
                    case 3:
                        colorShader = std::make_unique<graphics::Shader>(*graphics,
                                                                         std::vector<std::uint8_t>(std::begin(ColorPSGLES3_glsl),
                                                                                                   std::end(ColorPSGLES3_glsl)),
                                                                         std::vector<std::uint8_t>(std::begin(ColorVSGLES3_glsl),
                                                                                                   std::end(ColorVSGLES3_glsl)),
                                                                         std::set<graphics::Vertex::Attribute::Semantic>{
                                                                             {graphics::Vertex::Attribute::Usage::position},
                                                                             {graphics::Vertex::Attribute::Usage::color}
                                                                         },
                                                                         std::vector<std::pair<std::string, graphics::DataType>>{
                                                                             {"color", graphics::DataType::float32Vector4}
                                                                         },
                                                                         std::vector<std::pair<std::string, graphics::DataType>>{
                                                                             {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                         });
                        break;
#  else
                    case 2:
                        colorShader = std::make_unique<graphics::Shader>(*graphics,
                                                                         std::vector<std::uint8_t>(std::begin(ColorPSGL2_glsl),
                                                                                                   std::end(ColorPSGL2_glsl)),
                                                                         std::vector<std::uint8_t>(std::begin(ColorVSGL2_glsl),
                                                                                                   std::end(ColorVSGL2_glsl)),
                                                                         std::set<graphics::Vertex::Attribute::Semantic>{
                                                                             {graphics::Vertex::Attribute::Usage::position},
                                                                             {graphics::Vertex::Attribute::Usage::color}
                                                                         },
                                                                         std::vector<std::pair<std::string, graphics::DataType>>{
                                                                             {"color", graphics::DataType::float32Vector4}
                                                                         },
                                                                         std::vector<std::pair<std::string, graphics::DataType>>{
                                                                             {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                         });
                        break;
                    case 3:
                        colorShader = std::make_unique<graphics::Shader>(*graphics,
                                                                         std::vector<std::uint8_t>(std::begin(ColorPSGL3_glsl),
                                                                                                   std::end(ColorPSGL3_glsl)),
                                                                         std::vector<std::uint8_t>(std::begin(ColorVSGL3_glsl),
                                                                                                   std::end(ColorVSGL3_glsl)),
                                                                         std::set<graphics::Vertex::Attribute::Semantic>{
                                                                             {graphics::Vertex::Attribute::Usage::position},
                                                                             {graphics::Vertex::Attribute::Usage::color}
                                                                         },
                                                                         std::vector<std::pair<std::string, graphics::DataType>>{
                                                                             {"color", graphics::DataType::float32Vector4}
                                                                         },
                                                                         std::vector<std::pair<std::string, graphics::DataType>>{
                                                                             {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                         });
                        break;
                    case 4:
                        colorShader = std::make_unique<graphics::Shader>(*graphics,
                                                                         std::vector<std::uint8_t>(std::begin(ColorPSGL4_glsl),
                                                                                                   std::end(ColorPSGL4_glsl)),
                                                                         std::vector<std::uint8_t>(std::begin(ColorVSGL4_glsl),
                                                                                                   std::end(ColorVSGL4_glsl)),
                                                                         std::set<graphics::Vertex::Attribute::Semantic>{
                                                                             {graphics::Vertex::Attribute::Usage::position},
                                                                             {graphics::Vertex::Attribute::Usage::color}
                                                                         },
                                                                         std::vector<std::pair<std::string, graphics::DataType>>{
                                                                             {"color", graphics::DataType::float32Vector4}
                                                                         },
                                                                         std::vector<std::pair<std::string, graphics::DataType>>{
                                                                             {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                         });
                        break;
#  endif
                    default:
                        throw std::runtime_error("Unsupported OpenGL version");
                }

                assetBundle.setShader(shaderColor, std::move(colorShader));
                break;
            }
#endif

#if OUZEL_COMPILE_DIRECT3D11
            case graphics::Driver::direct3D11:
            {
                auto textureShader = std::make_unique<graphics::Shader>(*graphics,
                                                                        std::vector<std::uint8_t>(std::begin(TEXTURE_PIXEL_SHADER_D3D11),
                                                                                                  std::end(TEXTURE_PIXEL_SHADER_D3D11)),
                                                                        std::vector<std::uint8_t>(std::begin(TEXTURE_VERTEX_SHADER_D3D11),
                                                                                                  std::end(TEXTURE_VERTEX_SHADER_D3D11)),
                                                                        std::set<graphics::Vertex::Attribute::Semantic>{
                                                                            {graphics::Vertex::Attribute::Usage::position},
                                                                            {graphics::Vertex::Attribute::Usage::color},
                                                                            {graphics::Vertex::Attribute::Usage::textureCoordinates, 0U}
                                                                        },
                                                                        std::vector<std::pair<std::string, graphics::DataType>>{
                                                                            {"color", graphics::DataType::float32Vector4}
                                                                        },
                                                                        std::vector<std::pair<std::string, graphics::DataType>>{
                                                                            {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                        });

                assetBundle.setShader(shaderTexture, std::move(textureShader));

                auto colorShader = std::make_unique<graphics::Shader>(*graphics,
                                                                      std::vector<std::uint8_t>(std::begin(COLOR_PIXEL_SHADER_D3D11),
                                                                                                std::end(COLOR_PIXEL_SHADER_D3D11)),
                                                                      std::vector<std::uint8_t>(std::begin(COLOR_VERTEX_SHADER_D3D11),
                                                                                                std::end(COLOR_VERTEX_SHADER_D3D11)),
                                                                      std::set<graphics::Vertex::Attribute::Semantic>{
                                                                          {graphics::Vertex::Attribute::Usage::position},
                                                                          {graphics::Vertex::Attribute::Usage::color}
                                                                      },
                                                                      std::vector<std::pair<std::string, graphics::DataType>>{
                                                                          {"color", graphics::DataType::float32Vector4}
                                                                      },
                                                                      std::vector<std::pair<std::string, graphics::DataType>>{
                                                                          {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                      });

                assetBundle.setShader(shaderColor, std::move(colorShader));
                break;
            }
#endif

#if OUZEL_COMPILE_METAL
            case graphics::Driver::metal:
            {
                auto textureShader = std::make_unique<graphics::Shader>(*graphics,
                                                                        std::vector<std::uint8_t>(std::begin(TEXTURE_PIXEL_SHADER_METAL),
                                                                                                  std::end(TEXTURE_PIXEL_SHADER_METAL)),
                                                                        std::vector<std::uint8_t>(std::begin(TEXTURE_VERTEX_SHADER_METAL),
                                                                                                  std::end(TEXTURE_VERTEX_SHADER_METAL)),
                                                                        std::set<graphics::Vertex::Attribute::Semantic>{
                                                                            {graphics::Vertex::Attribute::Usage::position},
                                                                            {graphics::Vertex::Attribute::Usage::color},
                                                                            {graphics::Vertex::Attribute::Usage::textureCoordinates, 0U}
                                                                        },
                                                                        std::vector<std::pair<std::string, graphics::DataType>>{
                                                                            {"color", graphics::DataType::float32Vector4}
                                                                        },
                                                                        std::vector<std::pair<std::string, graphics::DataType>>{
                                                                            {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                        },
                                                                        "mainPS", "mainVS");

                assetBundle.setShader(shaderTexture, std::move(textureShader));

                auto colorShader = std::make_unique<graphics::Shader>(*graphics,
                                                                      std::vector<std::uint8_t>(std::begin(COLOR_PIXEL_SHADER_METAL),
                                                                                                std::end(COLOR_PIXEL_SHADER_METAL)),
                                                                      std::vector<std::uint8_t>(std::begin(COLOR_VERTEX_SHADER_METAL),
                                                                                                std::end(COLOR_VERTEX_SHADER_METAL)),
                                                                      std::set<graphics::Vertex::Attribute::Semantic>{
                                                                          {graphics::Vertex::Attribute::Usage::position},
                                                                          {graphics::Vertex::Attribute::Usage::color}
                                                                      },
                                                                      std::vector<std::pair<std::string, graphics::DataType>>{
                                                                          {"color", graphics::DataType::float32Vector4}
                                                                      },
                                                                      std::vector<std::pair<std::string, graphics::DataType>>{
                                                                          {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                      },
                                                                      "mainPS", "mainVS");

                assetBundle.setShader(shaderColor, std::move(colorShader));
                break;
            }
#endif

            default:
            {
                auto textureShader = std::make_unique<graphics::Shader>(*graphics,
                                                                        std::vector<std::uint8_t>(),
                                                                        std::vector<std::uint8_t>(),
                                                                        std::set<graphics::Vertex::Attribute::Semantic>{
                                                                            {graphics::Vertex::Attribute::Usage::position},
                                                                            {graphics::Vertex::Attribute::Usage::color},
                                                                            {graphics::Vertex::Attribute::Usage::textureCoordinates, 0U}
                                                                        },
                                                                        std::vector<std::pair<std::string, graphics::DataType>>{
                                                                            {"color", graphics::DataType::float32Vector4}
                                                                        },
                                                                        std::vector<std::pair<std::string, graphics::DataType>>{
                                                                            {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                        });

                assetBundle.setShader(shaderTexture, std::move(textureShader));

                auto colorShader = std::make_unique<graphics::Shader>(*graphics,
                                                                      std::vector<std::uint8_t>(),
                                                                      std::vector<std::uint8_t>(),
                                                                      std::set<graphics::Vertex::Attribute::Semantic>{
                                                                          {graphics::Vertex::Attribute::Usage::position},
                                                                          {graphics::Vertex::Attribute::Usage::color}
                                                                      },
                                                                      std::vector<std::pair<std::string, graphics::DataType>>{
                                                                          {"color", graphics::DataType::float32Vector4}
                                                                      },
                                                                      std::vector<std::pair<std::string, graphics::DataType>>{
                                                                          {"modelViewProj", graphics::DataType::float32Matrix4}
                                                                      });

                assetBundle.setShader(shaderColor, std::move(colorShader));
                break;
            }
        }

        auto noBlendState = std::make_unique<graphics::BlendState>(*graphics,
                                                                   false,
                                                                   graphics::BlendFactor::one,
                                                                   graphics::BlendFactor::zero,
                                                                   graphics::BlendOperation::add,
                                                                   graphics::BlendFactor::one,
                                                                   graphics::BlendFactor::zero,
                                                                   graphics::BlendOperation::add);

        assetBundle.setBlendState(blendNoBlend, std::move(noBlendState));

        auto addBlendState = std::make_unique<graphics::BlendState>(*graphics,
                                                                    true,
                                                                    graphics::BlendFactor::one,
                                                                    graphics::BlendFactor::one,
                                                                    graphics::BlendOperation::add,
                                                                    graphics::BlendFactor::one,
                                                                    graphics::BlendFactor::one,
                                                                    graphics::BlendOperation::add);

        assetBundle.setBlendState(blendAdd, std::move(addBlendState));

        auto multiplyBlendState = std::make_unique<graphics::BlendState>(*graphics,
                                                                         true,
                                                                         graphics::BlendFactor::destColor,
                                                                         graphics::BlendFactor::zero,
                                                                         graphics::BlendOperation::add,
                                                                         graphics::BlendFactor::one,
                                                                         graphics::BlendFactor::one,
                                                                         graphics::BlendOperation::add);

        assetBundle.setBlendState(blendMultiply, std::move(multiplyBlendState));

        auto alphaBlendState = std::make_unique<graphics::BlendState>(*graphics,
                                                                      true,
                                                                      graphics::BlendFactor::srcAlpha,
                                                                      graphics::BlendFactor::invSrcAlpha,
                                                                      graphics::BlendOperation::add,
                                                                      graphics::BlendFactor::one,
                                                                      graphics::BlendFactor::one,
                                                                      graphics::BlendOperation::add);

        assetBundle.setBlendState(blendAlpha, std::move(alphaBlendState));

        auto screenBlendState = std::make_unique<graphics::BlendState>(*graphics,
                                                                       true,
                                                                       graphics::BlendFactor::one,
                                                                       graphics::BlendFactor::invSrcAlpha,
                                                                       graphics::BlendOperation::add,
                                                                       graphics::BlendFactor::one,
                                                                       graphics::BlendFactor::one,
                                                                       graphics::BlendOperation::add);

        assetBundle.setBlendState(blendScreen, std::move(screenBlendState));

        auto whitePixelTexture = std::make_shared<graphics::Texture>(*graphics,
                                                                     std::vector<std::uint8_t>{255, 255, 255, 255},
                                                                     math::Size<std::uint32_t, 2>{1U, 1U},
                                                                     graphics::Flags::none, 1);
        assetBundle.setTexture(textureWhitePixel, whitePixelTexture);
    }

    void Engine::start()
    {
        if (!active)
        {
            auto event = std::make_unique<SystemEvent>();
            event->type = Event::Type::engineStart;
            eventDispatcher.postEvent(std::move(event));

            active = true;
            paused = false;

            audio->start();

#ifndef __EMSCRIPTEN__
            updateThread = thread::Thread{&Engine::engineMain, this};
#else
            main(args);
#endif
        }
    }

    void Engine::pause()
    {
        if (active && !paused)
        {
            auto event = std::make_unique<SystemEvent>();
            event->type = Event::Type::enginePause;
            eventDispatcher.postEvent(std::move(event));

            paused = true;
        }
    }

    void Engine::resume()
    {
        if (active && paused)
        {
            auto event = std::make_unique<SystemEvent>();
            event->type = Event::Type::engineResume;
            eventDispatcher.postEvent(std::move(event));

            paused = false;

#ifndef __EMSCRIPTEN__
            updateCondition.notify_all();
#endif
        }
    }

    void Engine::exit()
    {
        paused = true;

        if (active)
        {
            auto event = std::make_unique<SystemEvent>();
            event->type = Event::Type::engineStop;
            eventDispatcher.postEvent(std::move(event));

            active = false;
        }

#ifndef __EMSCRIPTEN__
        if (updateThread.isJoinable() &&
            updateThread.getId() != std::this_thread::get_id())
        {
            std::unique_lock lock{updateMutex};
            updateCondition.notify_all();
            lock.unlock();
            updateThread.join();
        }
#endif
    }

    void Engine::update()
    {
        eventDispatcher.dispatchEvents();

        const auto currentTime = std::chrono::steady_clock::now();
        auto diff = currentTime - previousUpdateTime;

        if (diff > std::chrono::milliseconds(1)) // at least one millisecond has passed
        {
            if (diff > std::chrono::milliseconds(1000 / 20)) diff = std::chrono::milliseconds(1000 / 20); // limit the update rate to a minimum 20 FPS

            previousUpdateTime = currentTime;
            const auto delta = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(diff).count()) / 1000000.0F;

            auto updateEvent = std::make_unique<UpdateEvent>();
            updateEvent->type = Event::Type::update;
            updateEvent->delta = delta;
            eventDispatcher.dispatchEvent(std::move(updateEvent));
        }

        inputManager->update();
        window->update();
        audio->update();

        if (refillRenderQueue)
        {
            sceneManager.draw();
            refillRenderQueue = false;
        }

        refillRenderQueue = graphics->getRefillQueue(oneUpdatePerFrame);

    }

    void Engine::executeOnMainThread(const std::function<void()>& func)
    {
        if (active) runOnMainThread(func);
    }

    void Engine::engineMain()
    {
        thread::setCurrentThreadName("Application");

        try
        {
            std::unique_ptr<Application> application = ouzel::main(args);

#ifndef __EMSCRIPTEN__
            while (active)
            {
                if (!paused)
                    update();
                else
                {
                    std::unique_lock lock{updateMutex};
                    updateCondition.wait(lock, [this]() noexcept { return !active || !paused; });
                }
            }

            eventDispatcher.dispatchEvents();
#endif
        }
        catch (const std::exception& e)
        {
            logger.log() << Log::Level::error << e.what();
            exit();
        }
    }

    void Engine::openUrl(const std::string&)
    {
    }

    void Engine::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        screenSaverEnabled = newScreenSaverEnabled;
    }
}
