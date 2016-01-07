// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "Vector2.h"
#include "Size2.h"
#include "Input.h"

namespace ouzel
{
    struct Event
    {
        enum class Type
        {
            KEY_DOWN,
            KEY_UP,
            MOUSE_DOWN,
            MOUSE_UP,
            MOUSE_SCROLL,
            MOUSE_MOVE,
            MOUSE_DRAG,
            TOUCH_BEGIN,
            TOUCH_MOVE,
            TOUCH_END,
            TOUCH_CANCEL,
            GAMEPAD_BUTTON_DOWN,
            GAMEPAD_BUTTON_UP,
            SCREEN_SIZE
        };
        
        Type type;
        
        bool shiftDown = false;
        bool altDown = false;
        bool controlDown = false;
        bool commandDown = false;
        bool functionDown = false;
    };
    
    struct KeyboardEvent: public Event
    {
        KeyboardKey key = KeyboardKey::NONE;
    };
    
    struct MouseEvent: public Event
    {
        MouseButton button = MouseButton::NONE;
        Vector2 position;
        Vector2 scroll;
    };
    
    struct TouchEvent: public Event
    {
        uint64_t touchId = 0;
        Vector2 position;
    };
    
    struct GamepadEvent: public Event
    {
        uint64_t gamepadId = 0;
        GamepadButton button;
    };
    
    struct ScreenSizeEvent: public Event
    {
        Size2 size;
    };
}
