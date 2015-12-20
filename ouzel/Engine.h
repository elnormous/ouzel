// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Renderer.h"
#include "EventHander.h"

namespace ouzel
{
    class Renderer;
    class Scene;
    
    class Engine: public Noncopyable, public ReferenceCounted
    {
    public:
        Engine(Renderer::Driver driver, const Size2& size, bool fullscreen = false);
        virtual ~Engine();
        
        void begin();
        void run();
        
        Renderer* getRenderer() const { return _renderer; }
        Scene* getScene() const { return _scene; }
        
        void addEventHandler(EventHandler* eventHandler);
        void removeEventHandler(EventHandler* eventHandler);
        
        void handleEvent(const Event& event);
        
    protected:
        Renderer* _renderer = nullptr;
        Scene* _scene = nullptr;
        
        long _previousFrameTime;
        
        std::vector<EventHandler*> _eventHandlers;
    };
}
