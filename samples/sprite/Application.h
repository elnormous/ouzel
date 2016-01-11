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
        
        bool handleKeyDown(const KeyboardEvent& event, std::shared_ptr<void> const& sender) const;
        bool handleMouseMove(const MouseEvent& event, std::shared_ptr<void> const& sender) const;
        bool handleTouch(const TouchEvent& event, std::shared_ptr<void> const& sender) const;
        bool handleGamepadButtonChange(const GamepadEvent& event, std::shared_ptr<void> const& sender) const;
        
    protected:
        std::shared_ptr<Layer> _layer;
        std::shared_ptr<Layer> _uiLayer;
        
        std::shared_ptr<Sprite> _sprite;
        std::shared_ptr<Sprite> _witch;
        
        std::shared_ptr<Button> _button;
        
        std::shared_ptr<EventHandler> _eventHandler;
    };
}
