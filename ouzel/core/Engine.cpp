// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <thread>
#include "Engine.h"
#include "CompileConfig.h"
#include "Cache.h"
#include "localization/Localization.h"
#include "utils/Utils.h"
#include "graphics/Renderer.h"
#include "audio/Audio.h"
#include "files/FileSystem.h"

#if OUZEL_PLATFORM_MACOS
#include "macos/WindowMacOS.h"
#include "macos/RendererOGLMacOS.h"
#elif OUZEL_PLATFORM_IOS
#include "ios/WindowIOS.h"
#include "ios/RendererOGLIOS.h"
#elif OUZEL_PLATFORM_TVOS
#include "tvos/WindowTVOS.h"
#include "tvos/RendererOGLTVOS.h"
#elif OUZEL_PLATFORM_ANDROID
#include "android/WindowAndroid.h"
#include "android/InputAndroid.h"
#elif OUZEL_PLATFORM_LINUX
#include "linux/WindowLinux.h"
#include "linux/RendererOGLLinux.h"
#include "linux/InputLinux.h"
#elif OUZEL_PLATFORM_WINDOWS
#include "win/WindowWin.h"
#elif OUZEL_PLATFORM_RASPBIAN
#include "rpi/WindowRPI.h"
#include "rpi/RendererOGLRPI.h"
#include "rpi/InputRPI.h"
#endif

#if OUZEL_SUPPORTS_OPENGL || OUZEL_SUPPORTS_OPENGLES
#include "opengl/RendererOGL.h"
#endif

#if OUZEL_SUPPORTS_DIRECT3D11
#include "direct3d11/RendererD3D11.h"
#endif

#if OUZEL_SUPPORTS_METAL
#include "metal/RendererMetal.h"
#endif

#if OUZEL_SUPPORTS_OPENAL
#include "openal/AudioAL.h"
#endif

#if OUZEL_SUPPORTS_XAUDIO2
#include "xaudio2/AudioXA2.h"
#endif

#if OUZEL_SUPPORTS_OPENSL
#include "opensl/AudioSL.h"
#endif

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include "apple/AudioALApple.h"
#include "apple/InputApple.h"
#elif OUZEL_PLATFORM_WINDOWS
#include "win/InputWin.h"
#endif

namespace ouzel
{
    ouzel::Engine* sharedEngine = nullptr;

    Engine::Engine():
        updateThread(std::bind(&Engine::run, this)), running(false), active(true)
    {
        sharedEngine = this;
    }

    Engine::~Engine()
    {
        running = false;
        active = false;
        if (sceneManager)
        {
            sceneManager->setScene(nullptr);
        }
    }

    std::set<graphics::Renderer::Driver> Engine::getAvailableDrivers()
    {
        static std::set<graphics::Renderer::Driver> availableDrivers;

        if (availableDrivers.empty())
        {
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

    bool Engine::init(Settings& newSettings)
    {
        settings = newSettings;

        if (!settings.verticalSync && settings.targetFPS > 0.0f)
        {
            targetFrameInterval = static_cast<uint64_t>(1000000L * (1.0f / settings.targetFPS));
        }
        else
        {
            targetFrameInterval = 0L;
        }

        if (settings.driver == graphics::Renderer::Driver::DEFAULT)
        {
            auto availableDrivers = getAvailableDrivers();

            if (availableDrivers.find(graphics::Renderer::Driver::METAL) != availableDrivers.end())
            {
                settings.driver = graphics::Renderer::Driver::METAL;
            }
            else if (availableDrivers.find(graphics::Renderer::Driver::DIRECT3D11) != availableDrivers.end())
            {
                settings.driver = graphics::Renderer::Driver::DIRECT3D11;
            }
            else if (availableDrivers.find(graphics::Renderer::Driver::OPENGL) != availableDrivers.end())
            {
                settings.driver = graphics::Renderer::Driver::OPENGL;
            }
            else
            {
                log("Failed to select render driver");
                return false;
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
        window.reset(new WindowRPI(settings.size, settings.resizable, settings.fullscreen, settings.title));
#endif

        eventDispatcher.reset(new EventDispatcher());
        cache.reset(new Cache());
        fileSystem.reset(new FileSystem());
        sceneManager.reset(new scene::SceneManager());

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        input.reset(new input::InputApple());
#elif OUZEL_PLATFORM_ANDROID
        input.reset(new input::InputAndroid());
#elif OUZEL_PLATFORM_LINUX
        input.reset(new input::InputLinux());
#elif OUZEL_PLATFORM_WINDOWS
        input.reset(new input::InputWin());
#elif OUZEL_PLATFORM_RASPBIAN
        input.reset(new input::InputRPI());
#else
        input.reset(new input::Input());
#endif

        localization.reset(new Localization());

        switch (settings.driver)
        {
#if OUZEL_SUPPORTS_OPENGL || OUZEL_SUPPORTS_OPENGLES
            case graphics::Renderer::Driver::OPENGL:
                log("Using OpenGL render driver");
    #if OUZEL_PLATFORM_MACOS
                renderer.reset(new graphics::RendererOGLMacOS());
    #elif OUZEL_PLATFORM_IOS
                renderer.reset(new graphics::RendererOGLIOS());
    #elif OUZEL_PLATFORM_TVOS
                renderer.reset(new graphics::RendererOGLTVOS());
    #elif OUZEL_PLATFORM_LINUX
                renderer.reset(new graphics::RendererOGLLinux());
    #elif OUZEL_PLATFORM_RASPBIAN
                renderer.reset(new graphics::RendererOGLRPI());
    #else
                renderer.reset(new graphics::RendererOGL());
    #endif
                break;
#endif
#if OUZEL_SUPPORTS_DIRECT3D11
            case graphics::Renderer::Driver::DIRECT3D11:
                log("Using Direct3D 11 render driver");
                renderer.reset(new graphics::RendererD3D11());
                break;
#endif
#if OUZEL_SUPPORTS_METAL
            case graphics::Renderer::Driver::METAL:
                log("Using Metal render driver");
                renderer.reset(new graphics::RendererMetal());
                break;
#endif
            default:
                log("Unsupported render driver");
                return false;
        }

        if (!window->init())
        {
            return false;
        }

        if (!renderer->init(window,
                            settings.sampleCount,
                            settings.textureFiltering,
                            settings.targetFPS,
                            settings.verticalSync))
        {
            return false;
        }

#if OUZEL_SUPPORTS_OPENAL
    #if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        audio.reset(new audio::AudioALApple());
    #else
        audio.reset(new audio::AudioAL());
    #endif
#elif OUZEL_SUPPORTS_XAUDIO2
        audio.reset(new audio::AudioXA2());
#elif OUZEL_SUPPORTS_OPENSL
        audio.reset(new audio::AudioSL());
#else
        audio.reset(new audio::Audio());
#endif

        if (!audio->init())
        {
            return false;
        }

        return true;
    }

    void Engine::exit()
    {
        running = false;
        active = false;
    }

    void Engine::begin()
    {
        previousUpdateTime = previousFrameTime = getCurrentMicroSeconds();
        running = true;
    }

    void Engine::end()
    {
        running = false;
    }

    void Engine::pause()
    {
        running = false;
    }

    void Engine::resume()
    {
        previousUpdateTime = previousFrameTime = getCurrentMicroSeconds();
        running = true;
    }

    void Engine::run()
    {
        while (active)
        {
            if (running)
            {
                uint64_t currentTime = getCurrentMicroSeconds();

                float delta = static_cast<float>((currentTime - previousUpdateTime)) / 1000000.0f;
                previousUpdateTime = currentTime;
                
                input->update();
                eventDispatcher->update();

                for (updateCallbackIterator = updateCallbacks.begin(); updateCallbackIterator != updateCallbacks.end();)
                {
                    updateCallbackDeleted = false;

                    const UpdateCallback* updateCallback = *updateCallbackIterator;
                    if (updateCallback && updateCallback->callback)
                    {
                        updateCallback->callback(delta);
                    }

                    // current element wasn't delete from the list
                    if (!updateCallbackDeleted)
                    {
                        ++updateCallbackIterator;
                    }
                }

                static int fps = 0;
                static float sum = 0.0f;
                sum += delta;
                fps++;

                if (sum >= 1.0f)
                {
                    printf("update: %d (%f)\n", fps, delta);
                    sum = 0.0f;
                    fps = 0;
                }
            }
        }
    }

    bool Engine::draw()
    {
        uint64_t currentTime = getCurrentMicroSeconds();

        float delta = static_cast<float>((currentTime - previousFrameTime)) / 1000000.0f;
        previousFrameTime = currentTime;

        if (delta > 0.0f)
        {
            currentFPS = 1.0f / delta;
        }

        renderer->clear();
        sceneManager->draw();
        renderer->present();

        static int fps = 0;
        static float sum = 0.0f;
        sum += delta;
        fps++;

        if (sum >= 1.0f)
        {
            printf("draw: %d (%f)\n", fps, delta);
            sum = 0.0f;
            fps = 0;
        }

        return active;
    }

    void Engine::scheduleUpdate(const UpdateCallback& callback)
    {
        std::list<const UpdateCallback*>::iterator i = std::find(updateCallbacks.begin(), updateCallbacks.end(), &callback);

        if (i == updateCallbacks.end())
        {
            updateCallbacks.push_back(&callback);

            updateCallbacks.sort([](const UpdateCallback* a, const UpdateCallback* b) {
                return a->priority < b->priority;
            });
        }
    }

    void Engine::unscheduleUpdate(const UpdateCallback& callback)
    {
        std::list<const UpdateCallback*>::iterator i = std::find(updateCallbacks.begin(), updateCallbacks.end(), &callback);

        if (i != updateCallbacks.end())
        {
            // increment the iterator if current element is deleted
            if (i == updateCallbackIterator)
            {
                updateCallbackIterator = updateCallbacks.erase(i);
                updateCallbackDeleted = true;
            }
            else
            {
                updateCallbacks.erase(i);
            }
        }
    }
}
