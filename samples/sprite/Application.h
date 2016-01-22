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
        
        bool handleKeyDown(const KeyboardEvent& event, const VoidPtr& sender) const;
        bool handleMouseMove(const MouseEvent& event, const VoidPtr& sender) const;
        bool handleTouch(const TouchEvent& event, const VoidPtr& sender) const;
        bool handleGamepadButtonChange(const GamepadEvent& event, const VoidPtr& sender) const;
        
    protected:
        LayerPtr _layer;
        LayerPtr _uiLayer;
        
        SpritePtr _sprite;
        SpritePtr _witch;
        
        ButtonPtr _button;
        
        EventHandlerPtr _eventHandler;
    };
}
