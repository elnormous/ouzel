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
        
        bool handleKeyboard(const KeyboardEventPtr& event, const VoidPtr& sender) const;
        bool handleMouse(const MouseEventPtr& event, const VoidPtr& sender) const;
        bool handleTouch(const TouchEventPtr& event, const VoidPtr& sender) const;
        bool handleGamepad(const GamepadEventPtr& event, const VoidPtr& sender) const;
        
    protected:
        LayerPtr _layer;
        LayerPtr _uiLayer;
        
        SpritePtr _sprite;
        SpritePtr _witch;
        ParticleSystemPtr _flame;
        
        ButtonPtr _button;
        
        EventHandlerPtr _eventHandler;
    };
}
