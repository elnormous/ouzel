// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Engine.h"
#include "CompileConfig.h"
#include "Cache.h"
#include "Window.h"
#include "localization/Localization.h"
#include "utils/Log.h"
#include "graphics/Renderer.h"
#include "audio/Audio.h"
#include "scene/SceneManager.h"
#include "events/EventDispatcher.h"

#if OUZEL_PLATFORM_MACOS
#include "macos/WindowMacOS.h"
#include "graphics/macos/RendererOGLMacOS.h"
#elif OUZEL_PLATFORM_IOS
#include "ios/WindowIOS.h"
#include "graphics/ios/RendererOGLIOS.h"
#elif OUZEL_PLATFORM_TVOS
#include "tvos/WindowTVOS.h"
#include "graphics/tvos/RendererOGLTVOS.h"
#elif OUZEL_PLATFORM_ANDROID
#include "android/WindowAndroid.h"
#include "input/android/InputAndroid.h"
#elif OUZEL_PLATFORM_LINUX
#include "linux/WindowLinux.h"
#include "graphics/linux/RendererOGLLinux.h"
#include "input/linux/InputLinux.h"
#elif OUZEL_PLATFORM_WINDOWS
#include "windows/WindowWin.h"
#elif OUZEL_PLATFORM_RASPBIAN
#include "raspbian/WindowRasp.h"
#include "graphics/raspbian/RendererOGLRasp.h"
#include "input/raspbian/InputRasp.h"
#elif OUZEL_PLATFORM_EMSCRIPTEN
#include "emscripten/WindowEm.h"
#include "graphics/emscripten/RendererOGLEm.h"
#include "input/emscripten/InputEm.h"
#endif

#include "graphics/empty/RendererEmpty.h"

#if OUZEL_SUPPORTS_OPENGL || OUZEL_SUPPORTS_OPENGLES
#include "graphics/opengl/RendererOGL.h"
#endif

#if OUZEL_SUPPORTS_DIRECT3D11
#include "graphics/direct3d11/RendererD3D11.h"
#endif

#if OUZEL_SUPPORTS_METAL
#include "graphics/metal/RendererMetal.h"
#endif

#include "audio/empty/AudioEmpty.h"

#if OUZEL_SUPPORTS_OPENAL
#include "audio/openal/AudioAL.h"
#endif

#if OUZEL_SUPPORTS_XAUDIO2
#include "audio/xaudio2/AudioXA2.h"
#endif

#if OUZEL_SUPPORTS_OPENSL
#include "audio/opensl/AudioSL.h"
#endif

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include "audio/apple/AudioALApple.h"
#include "input/apple/InputApple.h"
#elif OUZEL_PLATFORM_WINDOWS
#include "input/windows/InputWin.h"
#endif

#ifdef OPENAL
#undef OPENAL
#endif

namespace ouzel
{
    ouzel::Engine* sharedEngine = nullptr;

    Engine::Engine():
        currentFPS(0.0f), accumulatedFPS(0.0f), running(false), active(true)
    {
        sharedEngine = this;
    }

    Engine::~Engine()
    {
        running = false;
        active = false;

#if OUZEL_MULTITHREADED
        if (updateThread.joinable()) updateThread.join();
#endif

        sceneManager.reset();
    }

    std::set<graphics::Renderer::Driver> Engine::getAvailableRenderDrivers()
    {
        static std::set<graphics::Renderer::Driver> availableDrivers;

        if (availableDrivers.empty())
        {
            availableDrivers.insert(graphics::Renderer::Driver::EMPTY);

#if OUZEL_SUPPORTS_OPENGL || OUZEL_SUPPORTS_OPENGLES
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

#if OUZEL_SUPPORTS_XAUDIO2
            availableDrivers.insert(audio::Audio::Driver::XAUDIO2);
#endif

#if OUZEL_SUPPORTS_OPENSL
            availableDrivers.insert(audio::Audio::Driver::OPENSL);
#endif
        }

        return availableDrivers;
    }

    bool Engine::init(Settings& newSettings)
    {
        settings = newSettings;

        if (settings.renderDriver == graphics::Renderer::Driver::DEFAULT)
        {
            auto availableDrivers = getAvailableRenderDrivers();

            if (availableDrivers.find(graphics::Renderer::Driver::METAL) != availableDrivers.end())
            {
                settings.renderDriver = graphics::Renderer::Driver::METAL;
            }
            else if (availableDrivers.find(graphics::Renderer::Driver::DIRECT3D11) != availableDrivers.end())
            {
                settings.renderDriver = graphics::Renderer::Driver::DIRECT3D11;
            }
            else if (availableDrivers.find(graphics::Renderer::Driver::OPENGL) != availableDrivers.end())
            {
                settings.renderDriver = graphics::Renderer::Driver::OPENGL;
            }
            else
            {
                settings.renderDriver = graphics::Renderer::Driver::EMPTY;
            }
        }

#if OUZEL_PLATFORM_MACOS
        window.reset(new WindowMacOS(settings.size, settings.resizable, settings.fullscreen, settings.title));
#elif OUZEL_PLATFORM_IOS
        window.reset(new WindowIOS(settings.size, settings.resizable, settings.fullscreen, settings.title));
#elif OUZEL_PLATFORM_TVOS
        window.reset(new WindowTVOS(settings.size, settings.resizable, settings.fullscreen, settings.title));
#elif OUZEL_PLATFORM_ANDROID
        window.reset(new WindowAndroid(settings.size, settings.resizable, settings.fullscreen, settings.title));
#elif OUZEL_PLATFORM_LINUX
        window.reset(new WindowLinux(settings.size, settings.resizable, settings.fullscreen, settings.title));
#elif OUZEL_PLATFORM_WINDOWS
        window.reset(new WindowWin(settings.size, settings.resizable, settings.fullscreen, settings.title));
#elif OUZEL_PLATFORM_RASPBIAN
        window.reset(new WindowRasp(settings.size, settings.resizable, settings.fullscreen, settings.title));
#elif OUZEL_PLATFORM_EMSCRIPTEN
        window.reset(new WindowEm(settings.size, settings.resizable, settings.fullscreen, settings.title));
#else
        window.reset(new Window(settings.size, settings.resizable, settings.fullscreen, settings.title));
#endif

        eventDispatcher.reset(new EventDispatcher());
        cache.reset(new Cache());
        sceneManager.reset(new scene::SceneManager());

        localization.reset(new Localization());

        switch (settings.renderDriver)
        {
            case graphics::Renderer::Driver::EMPTY:
                Log(Log::Level::INFO) << "Not using render driver";
                renderer.reset(new graphics::RendererEmpty());
                break;
#if OUZEL_SUPPORTS_OPENGL || OUZEL_SUPPORTS_OPENGLES
            case graphics::Renderer::Driver::OPENGL:
                Log(Log::Level::INFO) << "Using OpenGL render driver";
    #if OUZEL_PLATFORM_MACOS
                renderer.reset(new graphics::RendererOGLMacOS());
    #elif OUZEL_PLATFORM_IOS
                renderer.reset(new graphics::RendererOGLIOS());
    #elif OUZEL_PLATFORM_TVOS
                renderer.reset(new graphics::RendererOGLTVOS());
    #elif OUZEL_PLATFORM_LINUX
                renderer.reset(new graphics::RendererOGLLinux());
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
                renderer.reset(new graphics::RendererMetal());
                break;
#endif
            default:
                Log(Log::Level::ERR) << "Unsupported render driver";
                return false;
        }

        if (!window->init())
        {
            return false;
        }

        if (!renderer->init(window.get(),
                            settings.sampleCount,
                            settings.textureFilter,
                            settings.backBufferFormat,
                            settings.verticalSync))
        {
            return false;
        }

        if (settings.audioDriver == audio::Audio::Driver::DEFAULT)
        {
            auto availableDrivers = getAvailableAudioDrivers();

            if (availableDrivers.find(audio::Audio::Driver::OPENAL) != availableDrivers.end())
            {
                settings.audioDriver = audio::Audio::Driver::OPENAL;
            }
            else if (availableDrivers.find(audio::Audio::Driver::XAUDIO2) != availableDrivers.end())
            {
                settings.audioDriver = audio::Audio::Driver::XAUDIO2;
            }
            else if (availableDrivers.find(audio::Audio::Driver::OPENSL) != availableDrivers.end())
            {
                settings.audioDriver = audio::Audio::Driver::OPENSL;
            }
            else
            {
                settings.audioDriver = audio::Audio::Driver::EMPTY;
            }
        }

        switch (settings.audioDriver)
        {
            case audio::Audio::Driver::EMPTY:
                Log(Log::Level::INFO) << "Not using audio driver";
                audio.reset(new audio::AudioEmpty());
                break;
#if OUZEL_SUPPORTS_OPENAL
            case audio::Audio::Driver::OPENAL:
                Log(Log::Level::INFO) << "Using OpenAL audio driver";
    #if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
                audio.reset(new audio::AudioALApple());
    #else
                audio.reset(new audio::AudioAL());
    #endif
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
            default:
                Log(Log::Level::ERR) << "Unsupported audio driver";
                return false;
        }

        if (!audio->init())
        {
            return false;
        }

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        input.reset(new input::InputApple());
#elif OUZEL_PLATFORM_ANDROID
        input.reset(new input::InputAndroid());
#elif OUZEL_PLATFORM_LINUX
        input.reset(new input::InputLinux(window.get()));
#elif OUZEL_PLATFORM_WINDOWS
        input.reset(new input::InputWin());
#elif OUZEL_PLATFORM_RASPBIAN
        input.reset(new input::InputRasp());
#elif OUZEL_PLATFORM_EMSCRIPTEN
        input.reset(new input::InputEm());
#else
        input.reset(new input::Input());
#endif

        return true;
    }

    void Engine::exit()
    {
        running = false;
        active = false;
    }

    void Engine::begin()
    {
        previousUpdateTime = previousFrameTime = std::chrono::steady_clock::now();
        running = true;

#if OUZEL_MULTITHREADED
        updateThread = std::thread(&Engine::run, this);
#endif
    }

    void Engine::end()
    {
        running = false;
        active = false;

#if OUZEL_MULTITHREADED
        if (updateThread.joinable()) updateThread.join();
#endif
    }

    void Engine::pause()
    {
        running = false;
    }

    void Engine::resume()
    {
        previousUpdateTime = previousFrameTime = std::chrono::steady_clock::now();
        running = true;
    }

    void Engine::run()
    {
#if OUZEL_MULTITHREADED
        while (active)
        {
#endif
            if (running)
            {
                std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
                auto diff = currentTime - previousUpdateTime;

                if (diff > std::chrono::milliseconds(1)) // at least one millisecond has passed
                {
                    previousUpdateTime = currentTime;
                    float delta = std::chrono::duration_cast<std::chrono::microseconds>(diff).count() / 1000000.0f;

                    eventDispatcher->dispatchEvents();

                    if (sharedEngine->getRenderer()->getRefillDrawQueue())
                    {
                        sceneManager->draw();
                        renderer->flushDrawCommands();
                    }

                    // erase all null update callbacks from the list
                    for (auto i = updateCallbacks.begin(); i != updateCallbacks.end();)
                    {
                        i = (*i) ? ++i : updateCallbacks.erase(i);
                    }

                    if (!updateCallbackAddSet.empty())
                    {
                        for (const UpdateCallback* updateCallback : updateCallbackAddSet)
                        {
                            auto i = std::find(updateCallbacks.begin(), updateCallbacks.end(), updateCallback);

                            if (i == updateCallbacks.end())
                            {
                                updateCallbacks.push_back(updateCallback);
                            }
                        }
                        
                        std::stable_sort(updateCallbacks.begin(), updateCallbacks.end(), [](const UpdateCallback* a, const UpdateCallback* b) {
                            return a->priority > b->priority;
                        });
                    }

                    for (const UpdateCallback* updateCallback : updateCallbacks)
                    {
                        if (updateCallback && updateCallback->callback)
                        {
                            updateCallback->callback(delta);
                        }
                    }
                }
            }
#if OUZEL_MULTITHREADED
        }
#endif
    }

    bool Engine::draw()
    {
#if !OUZEL_MULTITHREADED
        Engine::run();
#endif

        std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - previousFrameTime);
        previousFrameTime = currentTime;

        float delta = diff.count() / 1000000000.0f;

        if (delta > 0.0f)
        {
            currentFPS = 1.0f / delta;
        }

        accumulatedTime += delta;
        currentAccumulatedFPS += 1.0f;

        if (accumulatedTime > 1.0f)
        {
            accumulatedFPS = currentAccumulatedFPS;
            accumulatedTime = 0.0f;
            currentAccumulatedFPS = 0.0f;
        }

        if (!renderer->present())
        {
            return false;
        }

        return active;
    }

    void Engine::scheduleUpdate(const UpdateCallback& callback)
    {
        updateCallbackAddSet.insert(&callback);
    }

    void Engine::unscheduleUpdate(const UpdateCallback& callback)
    {
        auto vectorIterator = std::find(updateCallbacks.begin(), updateCallbacks.end(), &callback);

        if (vectorIterator != updateCallbacks.end())
        {
            *vectorIterator = nullptr;
        }

        auto setIterator = updateCallbackAddSet.find(&callback);

        if (setIterator != updateCallbackAddSet.end())
        {
            updateCallbackAddSet.erase(setIterator);
        }
    }
}
