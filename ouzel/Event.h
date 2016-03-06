// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "Types.h"
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
            KEY_REPEAT,
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
            WINDOW_TITLE_CHANGE,
            WINDOW_FULLSCREEN_CHANGE
        };
        
        enum Modifiers
        {
            SHIFT_DOWN          = 0x0001,
            ALT_DOWN            = 0x0002,
            CONTROL_DOWN        = 0x0004,
            COMMAND_DOWN        = 0x0008,
            FUNCTION_DOWN       = 0x0010,
            LEFT_MOUSE_DOWN     = 0x0020,
            RIGHT_MOUSE_DOWN    = 0x0040,
            MIDDLE_MOUSE_DOWN   = 0x0080,
        };
        
        Type type;
        uint32_t modifiers = 0;
    };
    
    struct KeyboardEvent: public Event
    {
        input::KeyboardKey key = input::KeyboardKey::NONE;
    };
    
    struct MouseEvent: public Event
    {
        input::MouseButton button = input::MouseButton::NONE;
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
        input::GamepadPtr gamepad;
        input::GamepadButton button = input::GamepadButton::NONE;
        bool pressed = false;
        float value = 0.0f;
    };
    
    struct WindowEvent: public Event
    {
        Size2 size;
        std::string title;
        bool fullscreen = false;
    };
}
