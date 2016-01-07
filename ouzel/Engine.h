// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
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
        static std::shared_ptr<Engine> getInstance();
        
        virtual ~Engine();
        
        void setArgs(std::vector<std::string> const& args);
        bool init();

        std::shared_ptr<EventDispatcher> const& getEventDispatcher() const { return _eventDispatcher; }
        std::shared_ptr<Renderer> const& getRenderer() const { return _renderer; }
        std::shared_ptr<SceneManager> const& getSceneManager() const { return _sceneManager; }
        std::shared_ptr<FileSystem> const& getFileSystem() const { return _fileSystem; }
        std::shared_ptr<Input> const& getInput() const { return _input; }
        
        void begin();
        void end();
        void run();
        
    protected:
        Engine();
        
        std::vector<std::string> _args;
        
        std::shared_ptr<App> _app;
        
        std::shared_ptr<EventDispatcher> _eventDispatcher;
        std::shared_ptr<Renderer> _renderer;
        std::shared_ptr<SceneManager> _sceneManager;
        std::shared_ptr<FileSystem> _fileSystem;
        std::shared_ptr<Input> _input;
        
        uint64_t _previousFrameTime;
    };
}
