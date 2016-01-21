// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <set>
#include "Types.h"
#include "Noncopyable.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "FileSystem.h"
#include "Input.h"
#include "EventDispatcher.h"
#include "App.h"

namespace ouzel
{
    class Engine: public Noncopyable
    {
    public:
        static EnginePtr getInstance();
        
        virtual ~Engine();
        
        void setArgs(std::vector<std::string> const& args);
        std::vector<std::string> const& getArgs() { return _args; }
        
        std::set<Renderer::Driver> getAvailableDrivers() const;
        
        bool init();

        EventDispatcherPtr const& getEventDispatcher() const { return _eventDispatcher; }
        CachePtr const& getCache() const { return _cache; }
        RendererPtr const& getRenderer() const { return _renderer; }
        SceneManagerPtr const& getSceneManager() const { return _sceneManager; }
        FileSystemPtr const& getFileSystem() const { return _fileSystem; }
        InputPtr const& getInput() const { return _input; }
        
        void begin();
        void end();
        void run();
        
    protected:
        Engine();
        
        std::vector<std::string> _args;
        
        AppPtr _app;
        
        EventDispatcherPtr _eventDispatcher;
        CachePtr _cache;
        RendererPtr _renderer;
        SceneManagerPtr _sceneManager;
        FileSystemPtr _fileSystem;
        InputPtr _input;
        
        uint64_t _previousFrameTime;
    };
}
