// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include "Types.h"
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
        
        virtual bool init(std::string const& normal, std::string const& selected, std::string const& pressed, std::string const& disabled, std::function<void(VoidPtr)> const& callback = std::function<void(VoidPtr)>());
        
        virtual void setEnabled(bool enabled) override;
        
    protected:
        bool handleMouseDown(const MouseEvent& event, VoidPtr const& sender);
        bool handleMouseUp(const MouseEvent& event, VoidPtr const& sender);
        bool handleMouseMove(const MouseEvent& event, VoidPtr const& sender);
        bool handleTouchBegin(const TouchEvent& event, VoidPtr const& sender);
        bool handleTouchMove(const TouchEvent& event, VoidPtr const& sender);
        bool handleTouchEnd(const TouchEvent& event, VoidPtr const& sender);
        bool handleGamepadButtonChange(const GamepadEvent& event, VoidPtr const& sender);
        
        void checkPointer(Vector2 const& worldLocation);
        void updateSprite();
        
        SpritePtr _normalSprite;
        SpritePtr _selectedSprite;
        SpritePtr _pressedSprite;
        SpritePtr _disabledSprite;
        
        std::function<void(VoidPtr)> _callback;
        
        EventHandlerPtr _eventHandler;
        
        bool _selected = false;
        bool _pointerOver = false;
        bool _pressed = false;
    };
}
