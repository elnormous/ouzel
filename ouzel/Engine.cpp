// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Engine.h"
#include "CompileConfig.h"

#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
#include "RendererOGL.h"
#endif

#if defined(OUZEL_PLATFORM_WINDOWS)
#include "RendererD3D11.h"
#endif

#include "Utils.h"
#include "Renderer.h"
#include "FileSystem.h"

#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
#include "InputApple.h"
#elif defined(OUZEL_PLATFORM_WINDOWS)
#include "InputWin.h"
#endif

void ouzelInit(ouzel::Settings&);
void ouzelBegin();
void ouzelEnd();

namespace ouzel
{
    static Engine* sharedEngine = nullptr;
    
    Engine* Engine::getInstance()
    {
        if (!sharedEngine)
        {
            Settings settings;

#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
            settings.driver = Renderer::Driver::OPENGL;
#elif defined(OUZEL_PLATFORM_WINDOWS)
            settings.driver = Renderer::Driver::DIRECT3D11;
#endif
            ouzelInit(settings);

            sharedEngine = new Engine();
            sharedEngine->init(settings);
        }
        
        return sharedEngine;
    }
    
    Engine::Engine()
    {
    }

    bool Engine::init(Settings const& settings)
    {
        _eventDispatcher.reset(new EventDispatcher());
        _fileSystem.reset(new FileSystem());
        _sceneManager.reset(new SceneManager());
        
#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
        _input.reset(new InputApple());
#else
        _input.reset(new Input());
#endif

        switch (settings.driver)
        {
#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
            case Renderer::Driver::OPENGL:
                _renderer.reset(new RendererOGL());
				break;
#endif
#ifdef OUZEL_PLATFORM_WINDOWS
            case Renderer::Driver::DIRECT3D11:
                _renderer.reset(new RendererD3D11());
                break;
#endif
            default:
                _renderer.reset(new Renderer());
                break;
        }

        if (!_renderer->init(settings.size, settings.resizable, settings.fullscreen, settings.driver))
        {
            return false;
        }
        
        _previousFrameTime = getCurrentMicroSeconds();

        return true;
    }
    
    Engine::~Engine()
    {
        
    }
    
    void Engine::begin()
    {
        ouzelBegin();
    }
    
    void Engine::end()
    {
        ouzelEnd();
    }
    
    void Engine::run()
    {
        uint64_t currentTime = getCurrentMicroSeconds();
        float delta = static_cast<float>((currentTime - _previousFrameTime)) / 1000000.0f;
        _previousFrameTime = currentTime;
        
        _sceneManager->update(delta);
        
        _renderer->begin();
        _renderer->clear();
        _sceneManager->draw();
        _renderer->flush();
    }
}
