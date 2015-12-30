// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "AutoPtr.h"
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Renderer.h"
#include "EventHander.h"

namespace ouzel
{
    class Renderer;
    class Scene;
    class FileSystem;
    
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
        
        Renderer* getRenderer() const { return _renderer; }
        Scene* getScene() const { return _scene; }
        FileSystem* getFileSystem() const { return _fileSystem; }
        
        void addEventHandler(EventHandler* eventHandler);
        void removeEventHandler(EventHandler* eventHandler);
        
        void handleEvent(const Event& event);
        
    protected:
        AutoPtr<Renderer> _renderer;
        AutoPtr<Scene> _scene;
        AutoPtr<FileSystem> _fileSystem;
        
        uint64_t _previousFrameTime;
        
        std::vector<EventHandler*> _eventHandlers;
    };
}
