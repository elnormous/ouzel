// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Application: public Noncopyable, public ReferenceCounted
    {
    public:
        Application();
        virtual ~Application();
        
        void handleKeyDown(const KeyboardEvent& event, ReferenceCounted* sender) const;
        void handleMouseMove(const MouseEvent& event, ReferenceCounted* sender) const;
        void handleTouch(const TouchEvent& event, ReferenceCounted* sender) const;
        
    protected:
        SharedPtr<Sprite> _sprite;
        SharedPtr<Sprite> _witch;
        
        SharedPtr<Layer> _layer;
        
        SharedPtr<EventHandler> _eventHandler;
    };
}
