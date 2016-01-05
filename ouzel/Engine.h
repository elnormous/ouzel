// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "AutoPtr.h"
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
    
    class Engine: public Noncopyable, public ReferenceCounted
    {
    public:
        static Engine* getInstance();
        
        Engine();
        virtual ~Engine();
        
        void begin();
        void run();
        
    protected:
        AutoPtr<EventDispatcher> _eventDispatcher;
        AutoPtr<Renderer> _renderer;
        AutoPtr<SceneManager> _sceneManager;
        AutoPtr<FileSystem> _fileSystem;
        AutoPtr<Input> _input;
        
        uint64_t _previousFrameTime;
    };
}
