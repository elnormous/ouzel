// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <memory>
#include <string>
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
            TOUCH_BEGIN,
            TOUCH_MOVE,
            TOUCH_END,
            TOUCH_CANCEL,
            GAMEPAD_CONNECT,
            GAMEPAD_DISCONNECT,
            GAMEPAD_BUTTON_CHANGE,
            WINDOW_SIZE_CHANGE,
            WINDOW_TITLE_CHANGE
        };
        
        Type type;
        
        bool shiftDown = false;
        bool altDown = false;
        bool controlDown = false;
        bool commandDown = false;
        bool functionDown = false;
        bool leftMouseDown = false;
        bool rightMouseDown = false;
        bool middleMouseDown = false;
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
    
    class Gamepad;
    
    struct GamepadEvent: public Event
    {
        std::shared_ptr<Gamepad> gamepad;
        GamepadButton button = GamepadButton::NONE;
        bool pressed = false;
        float value = 0.0f;
    };
    
    struct WindowEvent: public Event
    {
        Size2 size;
        std::string title;
    };
}
