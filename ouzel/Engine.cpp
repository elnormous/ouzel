// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Engine.h"
#include "CompileConfig.h"

#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS)
#include "RendererOGL.h"
#endif

#if defined(OUZEL_PLATFORM_WINDOWS)
#include "RendererD3D11.h"
#endif

#include "Utils.h"
#include "Renderer.h"
#include "Scene.h"
#include "FileSystem.h"

void ouzelInit(ouzel::Settings&);
void ouzelBegin();
void ouzelEnd();

namespace ouzel
{
    Engine* sharedEngine = nullptr;
    
    Engine* Engine::getInstance()
    {
        return sharedEngine;
    }
    
    Engine::Engine()
    {
        if (!sharedEngine)
        {
            sharedEngine = this;
        }
        else
        {
            log("Engine instance already created");
            abort();
        }
        
        Settings settings;
        
#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS)
        settings.driver = Renderer::Driver::OPENGL;
#elif defined(OUZEL_PLATFORM_WINDOWS)
        settings.driver = Renderer::Driver::DIRECT3D11;
#endif

        ouzelInit(settings);

        _fileSystem = new FileSystem();
        
        switch (settings.driver)
        {
#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS)
            case Renderer::Driver::OPENGL:
                _renderer = new RendererOGL(settings.size, settings.resizable, settings.fullscreen);
				break;
#endif
#ifdef OUZEL_PLATFORM_WINDOWS
            case Renderer::Driver::DIRECT3D11:
                _renderer = new RendererD3D11(settings.size, settings.resizable, settings.fullscreen);
                break;
#endif
            default:
                _renderer = new Renderer(settings.size, settings.resizable, settings.fullscreen);
                break;
        }
        
        _scene = new Scene();
        _scene->init();
        
        _previousFrameTime = getCurrentMicroSeconds();
    }
    
    Engine::~Engine()
    {
        ouzelEnd();
    }
    
    void Engine::begin()
    {
        ouzelBegin();
    }
    
    void Engine::run()
    {
        _renderer->begin();
        _renderer->clear();
        _scene->drawAll();
        _renderer->flush();
        
        uint64_t currentTime = getCurrentMicroSeconds();
        float delta = static_cast<float>((currentTime - _previousFrameTime)) / 1000000.0f;
        _previousFrameTime = currentTime;
        
        _scene->update(delta);
        
        for (EventHandler* eventHandler : _eventHandlers)
        {
            eventHandler->update(static_cast<float>(delta));
        }
    }
    
    void Engine::addEventHandler(EventHandler* eventHandler)
    {
        std::vector<EventHandler*>::iterator i = std::find(_eventHandlers.begin(), _eventHandlers.end(), eventHandler);
        
        if (i == _eventHandlers.end())
        {
            _eventHandlers.push_back(eventHandler);
        }
    }
    
    void Engine::removeEventHandler(EventHandler* eventHandler)
    {
        std::vector<EventHandler*>::iterator i = std::find(_eventHandlers.begin(), _eventHandlers.end(), eventHandler);
        
        if (i != _eventHandlers.end())
        {
            _eventHandlers.erase(i);
        }
    }
    
    void Engine::handleEvent(const Event& event)
    {
        for (EventHandler* eventHandler : _eventHandlers)
        {
            if (!eventHandler->handleEvent(event))
            {
                break;
            }
        }
    }
}
