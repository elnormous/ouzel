// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Engine.hpp"
#include "Setup.h"
#include "utils/Log.hpp"
#include "utils/INI.hpp"
#include "utils/Utils.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/RenderDevice.hpp"
#include "audio/Audio.hpp"
#include "thread/Lock.hpp"

#if OUZEL_PLATFORM_MACOS
#include "input/macos/InputMacOS.hpp"
#elif OUZEL_PLATFORM_IOS
#include "input/ios/InputIOS.hpp"
#elif OUZEL_PLATFORM_TVOS
#include "input/tvos/InputTVOS.hpp"
#elif OUZEL_PLATFORM_ANDROID
#include <jni.h>
#include "input/android/InputAndroid.hpp"
#elif OUZEL_PLATFORM_LINUX
#include "linux/WindowResourceLinux.hpp"
#include "input/linux/InputLinux.hpp"
#elif OUZEL_PLATFORM_WINDOWS
#include "input/windows/InputWin.hpp"
#elif OUZEL_PLATFORM_RASPBIAN
#include "input/raspbian/InputRasp.hpp"
#elif OUZEL_PLATFORM_EMSCRIPTEN
#include "input/emscripten/InputEm.hpp"
#endif

extern std::string APPLICATION_NAME;

namespace ouzel
{
    ouzel::Engine* engine = nullptr;

    Engine::Engine():
        cache(&fileSystem), active(false), paused(false), screenSaverEnabled(true)
    {
        engine = this;
    }

    Engine::~Engine()
    {
        if (active)
        {
            Event event;
            event.type = Event::Type::ENGINE_STOP;
            eventDispatcher.postEvent(event);
        }

        paused = true;
        active = false;

#if OUZEL_MULTITHREADED
        if (updateThread.isJoinable())
        {
            {
                Lock lock(updateMutex);
                updateCondition.signal();
            }

            updateThread.join();
        }
#endif

        engine = nullptr;
    }

    bool Engine::init()
    {
        Thread::setCurrentThreadName("Main");

        graphics::Renderer::Driver graphicsDriver = graphics::Renderer::Driver::DEFAULT;
        Size2 size;
        uint32_t sampleCount = 1; // MSAA sample count
        graphics::Texture::Filter textureFilter = graphics::Texture::Filter::POINT;
        uint32_t maxAnisotropy = 1;
        bool resizable = false;
        bool fullscreen = false;
        bool verticalSync = true;
        bool depth = false;
        bool debugRenderer = false;
        bool exclusiveFullscreen = false;
        bool highDpi = true; // should high DPI resolution be used
        audio::Audio::Driver audioDriver = audio::Audio::Driver::DEFAULT;
        bool debugAudio = false;

        defaultSettings.init("settings.ini");
        userSettings.init(fileSystem.getStorageDirectory() + FileSystem::DIRECTORY_SEPARATOR + "settings.ini");

        const ini::Section& userEngineSection = userSettings.getSection("engine");
        const ini::Section& defaultEngineSection = defaultSettings.getSection("engine");

        std::string graphicsDriverValue = userEngineSection.getValue("graphicsDriver", defaultEngineSection.getValue("graphicsDriver"));

        if (!graphicsDriverValue.empty())
        {
            if (graphicsDriverValue == "default")
            {
                graphicsDriver = ouzel::graphics::Renderer::Driver::DEFAULT;
            }
            else if (graphicsDriverValue == "empty")
            {
                graphicsDriver = ouzel::graphics::Renderer::Driver::EMPTY;
            }
            else if (graphicsDriverValue == "opengl")
            {
                graphicsDriver = ouzel::graphics::Renderer::Driver::OPENGL;
            }
            else if (graphicsDriverValue == "direct3d11")
            {
                graphicsDriver = ouzel::graphics::Renderer::Driver::DIRECT3D11;
            }
            else if (graphicsDriverValue == "metal")
            {
                graphicsDriver = ouzel::graphics::Renderer::Driver::METAL;
            }
            else
            {
                ouzel::Log(ouzel::Log::Level::WARN) << "Invalid graphics driver specified";
                return false;
            }
        }

        std::string widthValue = userEngineSection.getValue("width", defaultEngineSection.getValue("width"));
        if (!widthValue.empty()) size.width = std::stof(widthValue);

        std::string heightValue = userEngineSection.getValue("height", defaultEngineSection.getValue("height"));
        if (!heightValue.empty()) size.height = std::stof(heightValue);

        std::string sampleCountValue = userEngineSection.getValue("sampleCount", defaultEngineSection.getValue("sampleCount"));
        if (!sampleCountValue.empty()) sampleCount = static_cast<uint32_t>(std::stoul(sampleCountValue));

        std::string textureFilterValue = userEngineSection.getValue("textureFilter", defaultEngineSection.getValue("textureFilter"));
        if (!textureFilterValue.empty())
        {
            if (textureFilterValue == "point")
            {
                textureFilter = ouzel::graphics::Texture::Filter::POINT;
            }
            else if (textureFilterValue == "linear")
            {
                textureFilter = ouzel::graphics::Texture::Filter::LINEAR;
            }
            else if (textureFilterValue == "bilinear")
            {
                textureFilter = ouzel::graphics::Texture::Filter::BILINEAR;
            }
            else if (textureFilterValue == "trilinear")
            {
                textureFilter = ouzel::graphics::Texture::Filter::TRILINEAR;
            }
            else
            {
                ouzel::Log(ouzel::Log::Level::WARN) << "Invalid texture filter specified";
                return false;
            }
        }

        std::string maxAnisotropyValue = userEngineSection.getValue("maxAnisotropy", defaultEngineSection.getValue("maxAnisotropy"));
        if (!maxAnisotropyValue.empty()) maxAnisotropy = static_cast<uint32_t>(std::stoul(maxAnisotropyValue));

        std::string resizableValue = userEngineSection.getValue("resizable", defaultEngineSection.getValue("resizable"));
        if (!resizableValue.empty()) resizable = (resizableValue == "true" || resizableValue == "1" || resizableValue == "yes");

        std::string fullscreenValue = userEngineSection.getValue("fullscreen", defaultEngineSection.getValue("fullscreen"));
        if (!fullscreenValue.empty()) fullscreen = (fullscreenValue == "true" || fullscreenValue == "1" || fullscreenValue == "yes");

        std::string verticalSyncValue = userEngineSection.getValue("verticalSync", defaultEngineSection.getValue("verticalSync"));
        if (!verticalSyncValue.empty()) verticalSync = (verticalSyncValue == "true" || verticalSyncValue == "1" || verticalSyncValue == "yes");

        std::string exclusiveFullscreenValue = userEngineSection.getValue("exclusiveFullscreen", defaultEngineSection.getValue("exclusiveFullscreen"));
        if (!exclusiveFullscreenValue.empty()) exclusiveFullscreen = (exclusiveFullscreenValue == "true" || exclusiveFullscreenValue == "1" || exclusiveFullscreenValue == "yes");

        std::string depthValue = userEngineSection.getValue("depth", defaultEngineSection.getValue("depth"));
        if (!depthValue.empty()) depth = (depthValue == "true" || depthValue == "1" || depthValue == "yes");

        std::string debugRendererValue = userEngineSection.getValue("debugRenderer", defaultEngineSection.getValue("debugRenderer"));
        if (!debugRendererValue.empty()) debugRenderer = (debugRendererValue == "true" || debugRendererValue == "1" || debugRendererValue == "yes");

        std::string highDpiValue = userEngineSection.getValue("highDpi", defaultEngineSection.getValue("highDpi"));
        if (!highDpiValue.empty()) highDpi = (highDpiValue == "true" || highDpiValue == "1" || highDpiValue == "yes");

        std::string audioDriverValue = userEngineSection.getValue("audioDriver", defaultEngineSection.getValue("audioDriver"));

        if (!audioDriverValue.empty())
        {
            if (audioDriverValue == "default")
            {
                audioDriver = ouzel::audio::Audio::Driver::DEFAULT;
            }
            else if (audioDriverValue == "empty")
            {
                audioDriver = ouzel::audio::Audio::Driver::EMPTY;
            }
            else if (audioDriverValue == "openal")
            {
                audioDriver = ouzel::audio::Audio::Driver::OPENAL;
            }
            else if (audioDriverValue == "directsound")
            {
                audioDriver = ouzel::audio::Audio::Driver::DIRECTSOUND;
            }
            else if (audioDriverValue == "xaudio2")
            {
                audioDriver = ouzel::audio::Audio::Driver::XAUDIO2;
            }
            else if (audioDriverValue == "opensl")
            {
                audioDriver = ouzel::audio::Audio::Driver::OPENSL;
            }
            else if (audioDriverValue == "coreaudio")
            {
                audioDriver = ouzel::audio::Audio::Driver::COREAUDIO;
            }
            else if (audioDriverValue == "alsa")
            {
                audioDriver = ouzel::audio::Audio::Driver::ALSA;
            }
            else
            {
                ouzel::Log(ouzel::Log::Level::WARN) << "Invalid audio driver specified";
                return false;
            }
        }

        std::string debugAudioValue = userEngineSection.getValue("debugAudio", defaultEngineSection.getValue("debugAudio"));
        if (!debugAudioValue.empty()) debugAudio = (debugAudioValue == "true" || debugAudioValue == "1" || debugAudioValue == "yes");

        if (graphicsDriver == graphics::Renderer::Driver::DEFAULT)
        {
            auto availableDrivers = graphics::Renderer::getAvailableRenderDrivers();

            if (availableDrivers.find(graphics::Renderer::Driver::METAL) != availableDrivers.end())
            {
                graphicsDriver = graphics::Renderer::Driver::METAL;
            }
            else if (availableDrivers.find(graphics::Renderer::Driver::DIRECT3D11) != availableDrivers.end())
            {
                graphicsDriver = graphics::Renderer::Driver::DIRECT3D11;
            }
            else if (availableDrivers.find(graphics::Renderer::Driver::OPENGL) != availableDrivers.end())
            {
                graphicsDriver = graphics::Renderer::Driver::OPENGL;
            }
            else
            {
                graphicsDriver = graphics::Renderer::Driver::EMPTY;
            }
        }

        renderer.reset(new graphics::Renderer(graphicsDriver));

        if (!window.init(size,
                         resizable,
                         fullscreen,
                         exclusiveFullscreen,
                         APPLICATION_NAME,
                         highDpi,
                         depth))
        {
            return false;
        }

        if (!renderer->init(&window,
                            window.getResolution(),
                            sampleCount,
                            textureFilter,
                            maxAnisotropy,
                            verticalSync,
                            depth,
                            debugRenderer))
        {
            return false;
        }

        if (audioDriver == audio::Audio::Driver::DEFAULT)
        {
            auto availableDrivers = audio::Audio::getAvailableAudioDrivers();

            if (availableDrivers.find(audio::Audio::Driver::COREAUDIO) != availableDrivers.end())
            {
                audioDriver = audio::Audio::Driver::COREAUDIO;
            }
            else if (availableDrivers.find(audio::Audio::Driver::ALSA) != availableDrivers.end())
            {
                audioDriver = audio::Audio::Driver::ALSA;
            }
            else if (availableDrivers.find(audio::Audio::Driver::OPENAL) != availableDrivers.end())
            {
                audioDriver = audio::Audio::Driver::OPENAL;
            }
            else if (availableDrivers.find(audio::Audio::Driver::XAUDIO2) != availableDrivers.end())
            {
                audioDriver = audio::Audio::Driver::XAUDIO2;
            }
            else if (availableDrivers.find(audio::Audio::Driver::DIRECTSOUND) != availableDrivers.end())
            {
                audioDriver = audio::Audio::Driver::DIRECTSOUND;
            }
            else if (availableDrivers.find(audio::Audio::Driver::OPENSL) != availableDrivers.end())
            {
                audioDriver = audio::Audio::Driver::OPENSL;
            }
            else
            {
                audioDriver = audio::Audio::Driver::EMPTY;
            }
        }

        audio.reset(new audio::Audio(audioDriver));

        if (!audio->init(debugAudio))
        {
            return false;
        }

#if OUZEL_PLATFORM_MACOS
        input.reset(new input::InputMacOS());
#elif OUZEL_PLATFORM_IOS
        input.reset(new input::InputIOS());
#elif OUZEL_PLATFORM_TVOS
        input.reset(new input::InputTVOS());
#elif OUZEL_PLATFORM_ANDROID
        input.reset(new input::InputAndroid());
#elif OUZEL_PLATFORM_LINUX
        input.reset(new input::InputLinux());
#elif OUZEL_PLATFORM_WINDOWS
        input.reset(new input::InputWin());
#elif OUZEL_PLATFORM_RASPBIAN
        input.reset(new input::InputRasp());
#elif OUZEL_PLATFORM_EMSCRIPTEN
        input.reset(new input::InputEm());
#else
        input.reset(new input::Input());
#endif

        if (!input->init())
        {
            return false;
        }

        if (!network.init())
        {
            return false;
        }

        return true;
    }

    int Engine::run()
    {
        return EXIT_SUCCESS;
    }

    void Engine::start()
    {
        if (!active)
        {
            Event event;
            event.type = Event::Type::ENGINE_START;
            eventDispatcher.postEvent(event);

            active = true;
            paused = false;

#if OUZEL_MULTITHREADED
            updateThread = Thread(std::bind(&Engine::main, this), "Game");
#else
            main();
#endif
        }
    }

    void Engine::pause()
    {
        if (active && !paused)
        {
            Event event;
            event.type = Event::Type::ENGINE_PAUSE;
            eventDispatcher.postEvent(event);

            paused = true;
        }
    }

    void Engine::resume()
    {
        if (active && paused)
        {
            Event event;
            event.type = Event::Type::ENGINE_RESUME;
            eventDispatcher.postEvent(event);

            paused = false;

#if OUZEL_MULTITHREADED
            Lock lock(updateMutex);
            updateCondition.signal();
#endif
        }
    }

    void Engine::exit()
    {
        paused = true;

        if (active)
        {
            Event event;
            event.type = Event::Type::ENGINE_STOP;
            eventDispatcher.postEvent(event);

            active = false;
        }

#if OUZEL_MULTITHREADED
        if (updateThread.isJoinable() &&
            updateThread.getId() != Thread::getCurrentThreadId())
        {
            {
                Lock lock(updateMutex);
                updateCondition.signal();
            }

            updateThread.join();
        }
#endif
    }

    void Engine::update()
    {
            eventDispatcher.dispatchEvents();

            sceneManager.update();

            if (renderer->getDevice()->getRefillQueue())
            {
                sceneManager.draw();
                renderer->getDevice()->flushCommands();
            }

            audio->update();
    }

    void Engine::main()
    {
        ouzelMain(args);

#if OUZEL_MULTITHREADED
        while (active)
        {
            if (!paused)
            {
                update();

                // TODO: implement sleep to reduce the power consumption
            }
            else
            {
                Lock lock(updateMutex);
                while (active && paused)
                    updateCondition.wait(updateMutex);
            }
        }

        eventDispatcher.dispatchEvents();
#endif
    }

    bool Engine::openURL(const std::string&)
    {
        return false;
    }

    void Engine::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        screenSaverEnabled = newScreenSaverEnabled;
    }
}
