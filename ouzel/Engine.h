// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

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
        Engine(Renderer::Driver driver, uint32_t width = 0, uint32_t height = 0, bool fullscreen = false);
        virtual ~Engine();
        
        void run();
        
        Renderer* getRenderer() const { return _renderer; }
        Scene* getScene() const { return _scene; }
        
        void addEventHandler(EventHandler* eventHandler);
        void removeEventHandler(EventHandler* eventHandler);
        
        void handleEvent(const Event& event);
        
    private:
        Renderer* _renderer = nullptr;
        Scene* _scene = nullptr;
        
        std::vector<EventHandler*> _eventHandlers;
    };
}
