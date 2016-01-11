// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Widget.h"
#include "Event.h"

namespace ouzel
{
    class Sprite;
    class EventHandler;
    
    class Button: public Widget
    {
    public:
        Button();
        virtual ~Button();
        
        virtual bool init(std::string const& normal, std::string const& selected, std::string const& pressed);
        
    protected:
        bool handleMouseDown(const MouseEvent& event, std::shared_ptr<void> const& sender);
        bool handleMouseUp(const MouseEvent& event, std::shared_ptr<void> const& sender);
        bool handleMouseMove(const MouseEvent& event, std::shared_ptr<void> const& sender);
        bool handleTouchBegin(const TouchEvent& event, std::shared_ptr<void> const& sender);
        bool handleTouchMove(const TouchEvent& event, std::shared_ptr<void> const& sender);
        bool handleTouchEnd(const TouchEvent& event, std::shared_ptr<void> const& sender);
        bool handleGamepadButtonChange(const GamepadEvent& event, std::shared_ptr<void> const& sender);
        
        std::shared_ptr<Sprite> _normalSprite;
        std::shared_ptr<Sprite> _selectedSprite;
        std::shared_ptr<Sprite> _pressedSprite;
        
        std::shared_ptr<EventHandler> _eventHandler;
    };
}
