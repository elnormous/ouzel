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

namespace ouzel
{
    struct Settings
    {
        Renderer::Driver driver = Renderer::Driver::NONE;
        Size2 size;
        bool resizable = false;
        bool fullscreen = false;
    };
    
    class Engine: public Noncopyable
    {
    public:
        static Engine* getInstance();
        
        virtual ~Engine();
        
        std::shared_ptr<EventDispatcher> const& getEventDispatcher() const { return _eventDispatcher; }
        std::shared_ptr<Renderer> const& getRenderer() const { return _renderer; }
        std::shared_ptr<SceneManager> const& getSceneManager() const { return _sceneManager; }
        std::shared_ptr<FileSystem> const& getFileSystem() const { return _fileSystem; }
        std::shared_ptr<Input> const& getInput() const { return _input; }
        
        void begin();
        void run();
        void end();
        
    protected:
        Engine();
        
        std::shared_ptr<EventDispatcher> _eventDispatcher;
        std::shared_ptr<Renderer> _renderer;
        std::shared_ptr<SceneManager> _sceneManager;
        std::shared_ptr<FileSystem> _fileSystem;
        std::shared_ptr<Input> _input;
        
        uint64_t _previousFrameTime;
    };
}
