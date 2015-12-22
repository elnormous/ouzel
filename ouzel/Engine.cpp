// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Engine.h"
#include "CompileConfig.h"

#ifdef OUZEL_PLATFORM_OSX
#include "RendererOGL.h"
#endif

#ifdef OUZEL_PLATFORM_WINDOWS
#include "RendererD3D11.h"
#endif

#include "AppDelegate.h"
#include "Utils.h"
#include "Renderer.h"
#include "Scene.h"
#include "SoundManager.h"
#include "FileSystem.h"

extern ouzel::AppDelegate appDelegate;

namespace ouzel
{
    Engine::Engine(Renderer::Driver driver, const Size2& size, bool fullscreen)
    {
        switch (driver)
        {
            case Renderer::Driver::OPENGL:
#ifdef OUZEL_PLATFORM_OSX
                _renderer = new RendererOGL(size, fullscreen, this);
#endif
				break;
            case Renderer::Driver::DIRECT3D11:
#ifdef OUZEL_PLATFORM_WINDOWS
                _renderer = new RendererD3D11(size, fullscreen, this);
#endif
                break;
            default:
                _renderer = new Renderer(size, fullscreen, this);
                break;
        }
        
        _scene = new Scene(this);
        _scene->init();
        
        _soundManager = new SoundManager(this);
        
        _fileSystem = new FileSystem();
        
        _previousFrameTime = getCurrentMicroSeconds();
    }
    
    Engine::~Engine()
    {
        if (_renderer) _renderer->release();
        if (_scene) _scene->release();
        if (_fileSystem) _fileSystem->release();
    }
    
    void Engine::begin()
    {
        (&appDelegate)->begin(this);
    }
    
    void Engine::run()
    {
        _renderer->begin();
        _renderer->clear();
        _scene->drawAll();
        _renderer->flush();
        
        for (EventHandler* eventHandler : _eventHandlers)
        {
            uint64_t currentTime = getCurrentMicroSeconds();
            uint64_t delta = currentTime - _previousFrameTime;
            _previousFrameTime = currentTime;
            
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
