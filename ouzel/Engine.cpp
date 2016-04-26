// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Engine.h"
#include "CompileConfig.h"
#include "Cache.h"
#include "Localization.h"

#if defined(OUZEL_PLATFORM_OSX)
#include "osx/WindowOSX.h"
#elif defined(OUZEL_PLATFORM_IOS)
#include "ios/WindowIOS.h"
#elif defined(OUZEL_PLATFORM_TVOS)
#include "tvos/WindowTVOS.h"
#elif defined(OUZEL_PLATFORM_ANDROID)
#include "android/WindowAndroid.h"
#elif defined(OUZEL_PLATFORM_LINUX)
#include "linux/WindowLinux.h"
#elif defined(OUZEL_PLATFORM_WINDOWS)
#include "win/WindowWin.h"
#endif

#if defined(OUZEL_SUPPORTS_OPENGL) || defined(OUZEL_SUPPORTS_OPENGLES)
#include "opengl/RendererOGL.h"
#endif

#if defined(OUZEL_SUPPORTS_DIRECT3D11)
#include "direct3d11/RendererD3D11.h"
#endif

#if defined(OUZEL_SUPPORTS_METAL)
#include "metal/RendererMetal.h"
#endif

#include "Utils.h"
#include "Renderer.h"
#include "FileSystem.h"

#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
#include "apple/InputApple.h"
#elif defined(OUZEL_PLATFORM_WINDOWS)
#include "win/InputWin.h"
#endif

namespace ouzel
{
    ouzel::Engine* sharedEngine = nullptr;
    
    Engine::Engine()
    {
        sharedEngine = this;
    }

    Engine::~Engine()
    {
		_sceneManager->setScene(nullptr);
    }

    std::set<graphics::Renderer::Driver> Engine::getAvailableDrivers() const
    {
        std::set<graphics::Renderer::Driver> availableDrivers;

#if defined(OUZEL_SUPPORTS_OPENGL) || defined(OUZEL_SUPPORTS_OPENGLES)
        availableDrivers.insert(graphics::Renderer::Driver::OPENGL);
#endif

#if defined(OUZEL_SUPPORTS_DIRECT3D11)
        availableDrivers.insert(graphics::Renderer::Driver::DIRECT3D11);
#endif

#if defined(OUZEL_SUPPORTS_METAL)
        if (graphics::RendererMetal::available())
        {
            availableDrivers.insert(graphics::Renderer::Driver::METAL);
        }
#endif

        return availableDrivers;
    }

    bool Engine::init(Settings& settings)
    {
        _targetFPS = settings.targetFPS;

        if (settings.driver == graphics::Renderer::Driver::DEFAULT)
        {
            settings.driver = graphics::Renderer::Driver::NONE;

#if defined(OUZEL_SUPPORTS_METAL)
            if (graphics::RendererMetal::available())
            {
                settings.driver = graphics::Renderer::Driver::METAL;
            }
    #if defined(OUZEL_SUPPORTS_OPENGL) || defined(OUZEL_SUPPORTS_OPENGLES)
            else
            {
                settings.driver = graphics::Renderer::Driver::OPENGL;
            }
    #endif
#elif defined(OUZEL_SUPPORTS_DIRECT3D11)
            settings.driver = graphics::Renderer::Driver::DIRECT3D11;
#elif defined(OUZEL_SUPPORTS_OPENGL) || defined(OUZEL_SUPPORTS_OPENGLES)
            settings.driver = graphics::Renderer::Driver::OPENGL;
#endif
        }

#if defined(OUZEL_PLATFORM_OSX)
        _window.reset(new WindowOSX(settings.size, settings.resizable, settings.fullscreen, settings.title, settings.driver));
#elif defined(OUZEL_PLATFORM_IOS)
        _window.reset(new WindowIOS(settings.size, settings.resizable, settings.fullscreen, settings.title, settings.driver));
#elif defined(OUZEL_PLATFORM_TVOS)
        _window.reset(new WindowTVOS(settings.size, settings.resizable, settings.fullscreen, settings.title, settings.driver));
#elif defined(OUZEL_PLATFORM_ANDROID)
        _window.reset(new WindowAndroid(settings.size, settings.resizable, settings.fullscreen, settings.title, settings.driver));
#elif defined(OUZEL_PLATFORM_LINUX)
        _window.reset(new WindowLinux(settings.size, settings.resizable, settings.fullscreen, settings.title, settings.driver));
#elif defined(OUZEL_PLATFORM_WINDOWS)
        _window.reset(new WindowWin(settings.size, settings.resizable, settings.fullscreen, settings.title, settings.driver));
#endif

        _eventDispatcher.reset(new EventDispatcher());
        _cache.reset(new Cache());
        _fileSystem.reset(new FileSystem());
        _sceneManager.reset(new scene::SceneManager());

#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
        _input.reset(new input::InputApple());
#elif defined(OUZEL_PLATFORM_WINDOWS)
        _input.reset(new input::InputWin());
#else
        _input.reset(new input::Input());
#endif

        _localization.reset(new Localization());

        switch (settings.driver)
        {
            case graphics::Renderer::Driver::NONE:
                log("Using NULL render driver");
                _renderer.reset(new graphics::Renderer());
                break;
#if defined(OUZEL_SUPPORTS_OPENGL) || defined(OUZEL_SUPPORTS_OPENGLES)
            case graphics::Renderer::Driver::OPENGL:
                log("Using OpenGL render driver");
                _renderer.reset(new graphics::RendererOGL());
                break;
#endif
#if defined(OUZEL_SUPPORTS_DIRECT3D11)
            case graphics::Renderer::Driver::DIRECT3D11:
                log("Using Direct3D 11 render driver");
                _renderer.reset(new graphics::RendererD3D11());
                break;
#endif
#if defined(OUZEL_SUPPORTS_METAL)
            case graphics::Renderer::Driver::METAL:
                log("Using Metal render driver");
                _renderer.reset(new graphics::RendererMetal());
                break;
#endif
            default:
                log("Unsupported render driver");
                return false;
        }

        if (!_window->init())
        {
            return false;
        }

        _previousFrameTime = getCurrentMicroSeconds();

        return true;
    }

    void Engine::exit()
    {
        _active = false;
    }

    void Engine::begin()
    {
        _running = true;
    }

    void Engine::end()
    {

    }

    bool Engine::run()
    {
        uint64_t currentTime = getCurrentMicroSeconds();
        float delta = static_cast<float>((currentTime - _previousFrameTime)) / 1000000.0f;
        _previousFrameTime = currentTime;

        _currentFPS = 1.0f / delta;

        _input->update();
        _eventDispatcher->update();

        lock();
        for (const UpdateCallbackPtr& updateCallback : _updateCallbacks)
        {
            if (!updateCallback->_remove && updateCallback->callback)
            {
                updateCallback->callback(delta);
            }
        }
        unlock();

        _renderer->clear();
        _sceneManager->draw();
        _renderer->present();

        return _active;
    }

    void Engine::scheduleUpdate(const UpdateCallbackPtr& callback)
    {
        if (_locked)
        {
            _updateCallbackAddList.insert(callback);
        }
        else
        {
            std::vector<UpdateCallbackPtr>::iterator i = std::find(_updateCallbacks.begin(), _updateCallbacks.end(), callback);

            if (i == _updateCallbacks.end())
            {
                callback->_remove = false;
                _updateCallbacks.push_back(callback);
            }
        }
    }

    void Engine::unscheduleUpdate(const UpdateCallbackPtr& callback)
    {
        if (_locked)
        {
            callback->_remove = true;
            _updateCallbackRemoveList.insert(callback);
        }
        else
        {
            std::vector<UpdateCallbackPtr>::iterator i = std::find(_updateCallbacks.begin(), _updateCallbacks.end(), callback);

            if (i != _updateCallbacks.end())
            {
                _updateCallbacks.erase(i);
            }
        }
    }

    void Engine::lock()
    {
        ++_locked;
    }

    void Engine::unlock()
    {
        if (--_locked == 0)
        {
            if (!_updateCallbackAddList.empty())
            {
                for (const UpdateCallbackPtr& updateCallback : _updateCallbackAddList)
                {
                    scheduleUpdate(updateCallback);
                }
                _updateCallbackAddList.clear();
            }

            if (!_updateCallbackRemoveList.empty())
            {
                for (const UpdateCallbackPtr& updateCallback : _updateCallbackRemoveList)
                {
                    unscheduleUpdate(updateCallback);
                }
                _updateCallbackRemoveList.clear();
            }
        }
    }
}
