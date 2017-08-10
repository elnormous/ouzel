// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Engine.hpp"
#include "CompileConfig.h"
#include "Window.hpp"
#include "utils/Log.hpp"
#include "utils/INI.hpp"
#include "utils/Utils.hpp"
#include "graphics/Renderer.hpp"
#include "audio/Audio.hpp"

#if OUZEL_PLATFORM_MACOS
#include "macos/WindowMacOS.hpp"
#include "files/macos/FileSystemMacOS.hpp"
#include "graphics/metal/macos/RendererMetalMacOS.hpp"
#include "graphics/opengl/macos/RendererOGLMacOS.hpp"
#include "input/macos/InputMacOS.hpp"
#elif OUZEL_PLATFORM_IOS
#include "ios/WindowIOS.hpp"
#include "files/ios/FileSystemIOS.hpp"
#include "audio/openal/ios/AudioALIOS.hpp"
#include "graphics/metal/ios/RendererMetalIOS.hpp"
#include "graphics/opengl/ios/RendererOGLIOS.hpp"
#include "input/ios/InputIOS.hpp"
#elif OUZEL_PLATFORM_TVOS
#include "tvos/WindowTVOS.hpp"
#include "files/tvos/FileSystemTVOS.hpp"
#include "audio/openal/tvos/AudioALTVOS.hpp"
#include "graphics/metal/tvos/RendererMetalTVOS.hpp"
#include "graphics/opengl/tvos/RendererOGLTVOS.hpp"
#include "input/tvos/InputTVOS.hpp"
#elif OUZEL_PLATFORM_ANDROID
#include <jni.h>
#include "android/WindowAndroid.hpp"
#include "files/android/FileSystemAndroid.hpp"
#include "graphics/opengl/android/RendererOGLAndroid.hpp"
#include "input/android/InputAndroid.hpp"
#elif OUZEL_PLATFORM_LINUX
#include "linux/WindowLinux.hpp"
#include "files/linux/FileSystemLinux.hpp"
#include "graphics/opengl/linux/RendererOGLLinux.hpp"
#include "input/linux/InputLinux.hpp"
#elif OUZEL_PLATFORM_WINDOWS
#include "windows/WindowWin.hpp"
#include "files/windows/FileSystemWin.hpp"
#include "graphics/opengl/windows/RendererOGLWin.hpp"
#include "input/windows/InputWin.hpp"
#elif OUZEL_PLATFORM_RASPBIAN
#include "raspbian/WindowRasp.hpp"
#include "files/raspbian/FileSystemRasp.hpp"
#include "graphics/opengl/raspbian/RendererOGLRasp.hpp"
#include "input/raspbian/InputRasp.hpp"
#elif OUZEL_PLATFORM_EMSCRIPTEN
#include "emscripten/WindowEm.hpp"
#include "files/emscripten/FileSystemEm.hpp"
#include "graphics/opengl/emscripten/RendererOGLEm.hpp"
#include "input/emscripten/InputEm.hpp"
#endif

#include "graphics/empty/RendererEmpty.hpp"
#include "graphics/opengl/RendererOGL.hpp"
#include "graphics/direct3d11/RendererD3D11.hpp"
#include "graphics/metal/RendererMetal.hpp"

#include "audio/empty/AudioEmpty.hpp"
#include "audio/openal/AudioAL.hpp"
#include "audio/dsound/AudioDS.hpp"
#include "audio/xaudio2/AudioXA2.hpp"
#include "audio/opensl/AudioSL.hpp"
#include "audio/coreaudio/AudioCA.hpp"
#include "audio/alsa/AudioALSA.hpp"

extern std::string APPLICATION_NAME;

namespace ouzel
{
    ouzel::Engine* sharedEngine = nullptr;

    Engine::Engine():
        running(false), active(false), screenSaverEnabled(true)
    {
        sharedEngine = this;
    }

    Engine::~Engine()
    {
        if (active)
        {
            Event event;
            event.type = Event::Type::ENGINE_STOP;
            eventDispatcher.postEvent(event);
        }

        running = false;
        active = false;

#if OUZEL_MULTITHREADED
        updateCondition.notify_one();
        if (updateThread.joinable()) updateThread.join();
#endif

        for (UpdateCallback* updateCallback : updateCallbackAddSet)
        {
            updateCallback->engine = nullptr;
        }

        for (UpdateCallback* updateCallback : updateCallbacks)
        {
            auto i = std::find(updateCallbackDeleteSet.begin(), updateCallbackDeleteSet.end(), updateCallback);
            if (i == updateCallbackDeleteSet.end()) updateCallback->engine = nullptr;
        }

        sharedEngine = nullptr;
    }

    std::set<graphics::Renderer::Driver> Engine::getAvailableRenderDrivers()
    {
        static std::set<graphics::Renderer::Driver> availableDrivers;

        if (availableDrivers.empty())
        {
            availableDrivers.insert(graphics::Renderer::Driver::EMPTY);

#if OUZEL_SUPPORTS_OPENGL
            availableDrivers.insert(graphics::Renderer::Driver::OPENGL);
#endif

#if OUZEL_SUPPORTS_DIRECT3D11
            availableDrivers.insert(graphics::Renderer::Driver::DIRECT3D11);
#endif

#if OUZEL_SUPPORTS_METAL
            if (graphics::RendererMetal::available())
            {
                availableDrivers.insert(graphics::Renderer::Driver::METAL);
            }
#endif
        }

        return availableDrivers;
    }

    std::set<audio::Audio::Driver> Engine::getAvailableAudioDrivers()
    {
        static std::set<audio::Audio::Driver> availableDrivers;

        if (availableDrivers.empty())
        {
            availableDrivers.insert(audio::Audio::Driver::EMPTY);

#if OUZEL_SUPPORTS_OPENAL
            availableDrivers.insert(audio::Audio::Driver::OPENAL);
#endif

#if OUZEL_SUPPORTS_DIRECTSOUND
            availableDrivers.insert(audio::Audio::Driver::DIRECTSOUND);
#endif

#if OUZEL_SUPPORTS_XAUDIO2
            availableDrivers.insert(audio::Audio::Driver::XAUDIO2);
#endif

#if OUZEL_SUPPORTS_OPENSL
            availableDrivers.insert(audio::Audio::Driver::OPENSL);
#endif
#if OUZEL_SUPPORTS_COREAUDIO
            availableDrivers.insert(audio::Audio::Driver::COREAUDIO);
#endif
#if OUZEL_SUPPORTS_ALSA
            availableDrivers.insert(audio::Audio::Driver::ALSA);
#endif
        }

        return availableDrivers;
    }

    bool Engine::init()
    {
#if OUZEL_PLATFORM_MACOS
        fileSystem.reset(new FileSystemMacOS());
#elif OUZEL_PLATFORM_IOS
        fileSystem.reset(new FileSystemIOS());
#elif OUZEL_PLATFORM_TVOS
        fileSystem.reset(new FileSystemTVOS());
#elif OUZEL_PLATFORM_ANDROID
        fileSystem.reset(new FileSystemAndroid());
#elif OUZEL_PLATFORM_LINUX
        fileSystem.reset(new FileSystemLinux());
#elif OUZEL_PLATFORM_WINDOWS
        fileSystem.reset(new FileSystemWin());
#elif OUZEL_PLATFORM_RASPBIAN
        fileSystem.reset(new FileSystemRasp());
#elif OUZEL_PLATFORM_EMSCRIPTEN
        fileSystem.reset(new FileSystemEm());
#else
        fileSystem.reset(new FileSystem());
#endif

        graphics::Renderer::Driver graphicsDriver = graphics::Renderer::Driver::DEFAULT;
        audio::Audio::Driver audioDriver = audio::Audio::Driver::DEFAULT;

        Size2 size;
        uint32_t sampleCount = 1; // MSAA sample count
        graphics::Texture::Filter textureFilter = graphics::Texture::Filter::POINT;
        uint32_t maxAnisotropy = 1;
        bool resizable = false;
        bool fullscreen = false;
        bool verticalSync = true;
        bool depth = false;
        bool debugRenderer = false;
        bool highDpi = true; // should high DPI resolution be used
        bool debugAudio = false;

        defaultSettings.init("settings.ini");
        userSettings.init(fileSystem->getStorageDirectory() + FileSystem::DIRECTORY_SEPARATOR + "settings.ini");


        std::string graphicsDriverValue = userSettings.getValue("engine", "graphicsDriver", defaultSettings.getValue("engine", "graphicsDriver"));

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

        std::string audioDriverValue = userSettings.getValue("engine", "audioDriver", defaultSettings.getValue("engine", "audioDriver"));

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

        std::string widthValue = userSettings.getValue("engine", "width", defaultSettings.getValue("engine", "width"));
        if (!widthValue.empty()) size.v[0] = std::stof(widthValue);

        std::string heightValue = userSettings.getValue("engine", "height", defaultSettings.getValue("engine", "height"));
        if (!heightValue.empty()) size.v[1] = std::stof(heightValue);

        std::string sampleCountValue = userSettings.getValue("engine", "sampleCount", defaultSettings.getValue("engine", "sampleCount"));
        if (!sampleCountValue.empty()) sampleCount = static_cast<uint32_t>(std::stoul(sampleCountValue));

        std::string textureFilterValue = userSettings.getValue("engine", "textureFilter", defaultSettings.getValue("engine", "textureFilter"));
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

        std::string maxAnisotropyValue = userSettings.getValue("engine", "maxAnisotropy", defaultSettings.getValue("engine", "maxAnisotropy"));
        if (!maxAnisotropyValue.empty()) maxAnisotropy = static_cast<uint32_t>(std::stoul(maxAnisotropyValue));

        std::string resizableValue = userSettings.getValue("engine", "resizable", defaultSettings.getValue("engine", "resizable"));
        if (!resizableValue.empty()) resizable = (resizableValue == "true" || resizableValue == "1" || resizableValue == "yes");

        std::string fullscreenValue = userSettings.getValue("engine", "fullscreen", defaultSettings.getValue("engine", "fullscreen"));
        if (!fullscreenValue.empty()) fullscreen = (fullscreenValue == "true" || fullscreenValue == "1" || fullscreenValue == "yes");

        std::string verticalSyncValue = userSettings.getValue("engine", "verticalSync", defaultSettings.getValue("engine", "verticalSync"));
        if (!verticalSyncValue.empty()) verticalSync = (verticalSyncValue == "true" || verticalSyncValue == "1" || verticalSyncValue == "yes");

        std::string depthValue = userSettings.getValue("engine", "depth", defaultSettings.getValue("engine", "depth"));
        if (!depthValue.empty()) depth = (depthValue == "true" || depthValue == "1" || depthValue == "yes");

        std::string debugRendererValue = userSettings.getValue("engine", "debugRenderer", defaultSettings.getValue("engine", "debugRenderer"));
        if (!debugRendererValue.empty()) debugRenderer = (debugRendererValue == "true" || debugRendererValue == "1" || debugRendererValue == "yes");

        std::string highDpiValue = userSettings.getValue("engine", "highDpi", defaultSettings.getValue("engine", "highDpi"));
        if (!highDpiValue.empty()) highDpi = (highDpiValue == "true" || highDpiValue == "1" || highDpiValue == "yes");

        std::string debugAudioValue = userSettings.getValue("engine", "debugAudio", defaultSettings.getValue("engine", "debugAudio"));
        if (!debugAudioValue.empty()) debugAudio = (debugAudioValue == "true" || debugAudioValue == "1" || debugAudioValue == "yes");

        if (graphicsDriver == graphics::Renderer::Driver::DEFAULT)
        {
            auto availableDrivers = getAvailableRenderDrivers();

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

#if OUZEL_PLATFORM_MACOS
        window.reset(new WindowMacOS());
#elif OUZEL_PLATFORM_IOS
        window.reset(new WindowIOS());
#elif OUZEL_PLATFORM_TVOS
        window.reset(new WindowTVOS());
#elif OUZEL_PLATFORM_ANDROID
        window.reset(new WindowAndroid());
#elif OUZEL_PLATFORM_LINUX
        window.reset(new WindowLinux());
#elif OUZEL_PLATFORM_WINDOWS
        window.reset(new WindowWin());
#elif OUZEL_PLATFORM_RASPBIAN
        window.reset(new WindowRasp());
#elif OUZEL_PLATFORM_EMSCRIPTEN
        window.reset(new WindowEm());
#else
        window.reset(new Window());
#endif

        switch (graphicsDriver)
        {
            case graphics::Renderer::Driver::EMPTY:
                Log(Log::Level::INFO) << "Not using render driver";
                renderer.reset(new graphics::RendererEmpty());
                break;
#if OUZEL_SUPPORTS_OPENGL
            case graphics::Renderer::Driver::OPENGL:
                Log(Log::Level::INFO) << "Using OpenGL render driver";
    #if OUZEL_PLATFORM_MACOS
                renderer.reset(new graphics::RendererOGLMacOS());
    #elif OUZEL_PLATFORM_IOS
                renderer.reset(new graphics::RendererOGLIOS());
    #elif OUZEL_PLATFORM_TVOS
                renderer.reset(new graphics::RendererOGLTVOS());
    #elif OUZEL_PLATFORM_ANDROID
                renderer.reset(new graphics::RendererOGLAndroid());
    #elif OUZEL_PLATFORM_LINUX
                renderer.reset(new graphics::RendererOGLLinux());
    #elif OUZEL_PLATFORM_WINDOWS
                renderer.reset(new graphics::RendererOGLWin());
    #elif OUZEL_PLATFORM_RASPBIAN
                renderer.reset(new graphics::RendererOGLRasp());
    #elif OUZEL_PLATFORM_EMSCRIPTEN
                renderer.reset(new graphics::RendererOGLEm());
    #else
                renderer.reset(new graphics::RendererOGL());
    #endif
                break;
#endif
#if OUZEL_SUPPORTS_DIRECT3D11
            case graphics::Renderer::Driver::DIRECT3D11:
                Log(Log::Level::INFO) << "Using Direct3D 11 render driver";
                renderer.reset(new graphics::RendererD3D11());
                break;
#endif
#if OUZEL_SUPPORTS_METAL
            case graphics::Renderer::Driver::METAL:
                Log(Log::Level::INFO) << "Using Metal render driver";
    #if OUZEL_PLATFORM_MACOS
                renderer.reset(new graphics::RendererMetalMacOS());
    #elif OUZEL_PLATFORM_IOS
                renderer.reset(new graphics::RendererMetalIOS());
    #elif OUZEL_PLATFORM_TVOS
                renderer.reset(new graphics::RendererMetalTVOS());
    #endif
                break;
#endif
            default:
                Log(Log::Level::ERR) << "Unsupported render driver";
                return false;
        }

        if (!window->init(size,
                          resizable,
                          fullscreen,
                          APPLICATION_NAME,
                          highDpi,
                          depth))
        {
            return false;
        }

        if (!renderer->init(window.get(),
                            window->getSize() * window->getContentScale(),
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
            auto availableDrivers = getAvailableAudioDrivers();

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

        switch (audioDriver)
        {
            case audio::Audio::Driver::EMPTY:
                Log(Log::Level::INFO) << "Not using audio driver";
                audio.reset(new audio::AudioEmpty());
                break;
#if OUZEL_SUPPORTS_OPENAL
            case audio::Audio::Driver::OPENAL:
                Log(Log::Level::INFO) << "Using OpenAL audio driver";
    #if OUZEL_PLATFORM_IOS
                audio.reset(new audio::AudioALIOS());
    #elif OUZEL_PLATFORM_TVOS
                audio.reset(new audio::AudioALTVOS());
    #else
                audio.reset(new audio::AudioAL());
    #endif
                break;
#endif
#if OUZEL_SUPPORTS_DIRECTSOUND
            case audio::Audio::Driver::DIRECTSOUND:
                Log(Log::Level::INFO) << "Using DirectSound audio driver";
                audio.reset(new audio::AudioDS());
                break;
#endif
#if OUZEL_SUPPORTS_XAUDIO2
            case audio::Audio::Driver::XAUDIO2:
                Log(Log::Level::INFO) << "Using XAudio 2 audio driver";
                audio.reset(new audio::AudioXA2());
                break;
#endif
#if OUZEL_SUPPORTS_OPENSL
            case audio::Audio::Driver::OPENSL:
                Log(Log::Level::INFO) << "Using OpenSL ES audio driver";
                audio.reset(new audio::AudioSL());
                break;
#endif
#if OUZEL_SUPPORTS_COREAUDIO
            case audio::Audio::Driver::COREAUDIO:
                Log(Log::Level::INFO) << "Using CoreAudio audio driver";
                audio.reset(new audio::AudioCA());
                break;
#endif
#if OUZEL_SUPPORTS_ALSA
            case audio::Audio::Driver::ALSA:
                Log(Log::Level::INFO) << "Using ALSA audio driver";
                audio.reset(new audio::AudioALSA());
                break;
#endif
            default:
                Log(Log::Level::ERR) << "Unsupported audio driver";
                return false;
        }

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
            running = true;

            previousUpdateTime = std::chrono::steady_clock::now();

#if OUZEL_MULTITHREADED
            updateThread = std::thread(&Engine::main, this);
#else
            main();
#endif
        }
    }

    void Engine::pause()
    {
        if (active && running)
        {
            Event event;
            event.type = Event::Type::ENGINE_PAUSE;
            eventDispatcher.postEvent(event);

            running = false;
        }
    }

    void Engine::resume()
    {
        if (active && !running)
        {
            Event event;
            event.type = Event::Type::ENGINE_RESUME;
            eventDispatcher.postEvent(event);

            previousUpdateTime = std::chrono::steady_clock::now();
            running = true;

#if OUZEL_MULTITHREADED
            updateCondition.notify_one();
#endif
        }
    }

    void Engine::exit()
    {
        running = false;

        if (active)
        {
            Event event;
            event.type = Event::Type::ENGINE_STOP;
            eventDispatcher.postEvent(event);

            active = false;

#if OUZEL_MULTITHREADED
            updateCondition.notify_one();
#endif
        }
    }

    void Engine::update()
    {
        std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
        auto diff = currentTime - previousUpdateTime;

        if (diff > std::chrono::milliseconds(1)) // at least one millisecond has passed
        {
            previousUpdateTime = currentTime;
            float delta = std::chrono::duration_cast<std::chrono::microseconds>(diff).count() / 1000000.0f;

            eventDispatcher.dispatchEvents();
            timer.update(delta);

            for (UpdateCallback* updateCallback : updateCallbackDeleteSet)
            {
                auto i = std::find(updateCallbacks.begin(), updateCallbacks.end(), updateCallback);

                if (i != updateCallbacks.end())
                {
                    updateCallbacks.erase(i);
                }
            }

            updateCallbackDeleteSet.clear();

            for (UpdateCallback* updateCallback : updateCallbackAddSet)
            {
                auto i = std::find(updateCallbacks.begin(), updateCallbacks.end(), updateCallback);

                if (i == updateCallbacks.end())
                {
                    auto upperBound = std::upper_bound(updateCallbacks.begin(), updateCallbacks.end(), updateCallback,
                                                       [](const UpdateCallback* a, const UpdateCallback* b) {
                                                           return a->priority > b->priority;
                                                       });

                    updateCallbacks.insert(upperBound, updateCallback);
                }
            }

            updateCallbackAddSet.clear();

            for (UpdateCallback* updateCallback : updateCallbacks)
            {
                auto i = std::find(updateCallbackDeleteSet.begin(), updateCallbackDeleteSet.end(), updateCallback);

                if (i == updateCallbackDeleteSet.end())
                {
                    updateCallback->timeSinceLastUpdate += delta;

                    if (updateCallback->timeSinceLastUpdate >= updateCallback->interval)
                    {
                        updateCallback->timeSinceLastUpdate = (updateCallback->interval > 0.0f) ? fmodf(updateCallback->timeSinceLastUpdate, updateCallback->interval) : 0.0f;
                        if (updateCallback->callback) updateCallback->callback(delta);
                    }
                }
            }

            if (renderer->getRefillQueue())
            {
                sceneManager.draw();
                renderer->flushCommands();
            }
        }
    }

    void Engine::main()
    {
        ouzelMain(args);

#if OUZEL_MULTITHREADED
        while (active)
        {
            if (running)
            {
                update();

                // TODO: implement sleep to reduce the power consumption
            }
            else
            {
                std::unique_lock<std::mutex> lock(updateMutex);

                while (active && !running)
                {
                    updateCondition.wait(lock);
                }
            }
        }

        eventDispatcher.dispatchEvents();
#endif
    }

    void Engine::scheduleUpdate(UpdateCallback* callback)
    {
        if (callback->engine)
        {
            callback->engine->unscheduleUpdate(callback);
        }

        callback->engine = this;

        updateCallbackAddSet.insert(callback);

        auto setIterator = updateCallbackDeleteSet.find(callback);

        if (setIterator != updateCallbackDeleteSet.end())
        {
            updateCallbackDeleteSet.erase(setIterator);
        }
    }

    void Engine::unscheduleUpdate(UpdateCallback* callback)
    {
        if (callback->engine == this)
        {
            callback->engine = nullptr;
        }

        updateCallbackDeleteSet.insert(callback);

        auto setIterator = updateCallbackAddSet.find(callback);

        if (setIterator != updateCallbackAddSet.end())
        {
            updateCallbackAddSet.erase(setIterator);
        }
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
