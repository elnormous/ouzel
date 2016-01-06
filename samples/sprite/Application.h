// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Application: public Noncopyable
    {
    public:
        Application();
        virtual ~Application();
        
        void handleKeyDown(const KeyboardEvent& event, void* sender) const;
        void handleMouseMove(const MouseEvent& event, void* sender) const;
        void handleTouch(const TouchEvent& event, void* sender) const;
        
    protected:
        std::shared_ptr<Sprite> _sprite;
        std::shared_ptr<Sprite> _witch;
        
        std::shared_ptr<Layer> _layer;
        
        std::shared_ptr<EventHandler> _eventHandler;
    };
}
