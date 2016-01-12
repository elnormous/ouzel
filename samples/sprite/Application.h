// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Application: public Noncopyable, public App
    {
    public:
        virtual ~Application();
        
        virtual Settings getSettings() override;
        
        virtual void begin() override;
        
        bool handleKeyDown(const KeyboardEvent& event, VoidPtr const& sender) const;
        bool handleMouseMove(const MouseEvent& event, VoidPtr const& sender) const;
        bool handleTouch(const TouchEvent& event, VoidPtr const& sender) const;
        bool handleGamepadButtonChange(const GamepadEvent& event, VoidPtr const& sender) const;
        
    protected:
        LayerPtr _layer;
        LayerPtr _uiLayer;
        
        SpritePtr _sprite;
        SpritePtr _witch;
        
        ButtonPtr _button;
        
        EventHandlerPtr _eventHandler;
    };
}
