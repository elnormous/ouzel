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
        static std::shared_ptr<Button> create(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled, const std::function<void(const VoidPtr&)>& callback = std::function<void(const VoidPtr&)>());
        
        Button();
        virtual ~Button();
        
        virtual bool init(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled, const std::function<void(const VoidPtr&)>& callback = std::function<void(const VoidPtr&)>());
        
        virtual void setEnabled(bool enabled) override;
        
    protected:
        bool handleMouse(const MouseEventPtr& event, const VoidPtr& sender);
        bool handleTouch(const TouchEventPtr& event, const VoidPtr& sender);
        bool handleGamepad(const GamepadEventPtr& event, const VoidPtr& sender);
        
        void checkPointer(const Vector2& worldLocation);
        void updateSprite();
        
        SpritePtr _normalSprite;
        SpritePtr _selectedSprite;
        SpritePtr _pressedSprite;
        SpritePtr _disabledSprite;
        
        std::function<void(const VoidPtr&)> _callback;
        
        EventHandlerPtr _eventHandler;
        
        bool _selected = false;
        bool _pointerOver = false;
        bool _pressed = false;
    };
}
