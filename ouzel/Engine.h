// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "SharedPtr.h"
#include "Noncopyable.h"
#include "ReferenceCounted.h"
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
        
        void begin();
        void run();
        void end();
        
    protected:
        Engine();
        
        SharedPtr<EventDispatcher> _eventDispatcher;
        SharedPtr<Renderer> _renderer;
        SharedPtr<SceneManager> _sceneManager;
        SharedPtr<FileSystem> _fileSystem;
        SharedPtr<Input> _input;
        
        uint64_t _previousFrameTime;
    };
}
