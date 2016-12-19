// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "utils/Types.h"
#include "math/Vector2.h"
#include "math/Size2.h"
#include "input/Input.h"

namespace ouzel
{
    enum EventModifiers
    {
        SHIFT_DOWN          = 0x0001,
        ALT_DOWN            = 0x0002,
        CONTROL_DOWN        = 0x0004,
        SUPER_DOWN          = 0x0008, // Windows key on Windows, Command key on macOS
        FUNCTION_DOWN       = 0x0010,
        LEFT_MOUSE_DOWN     = 0x0020,
        RIGHT_MOUSE_DOWN    = 0x0040,
        MIDDLE_MOUSE_DOWN   = 0x0080,
    };

    struct KeyboardEvent
    {
        uint32_t modifiers = 0;
        input::KeyboardKey key = input::KeyboardKey::NONE;
    };

    struct MouseEvent
    {
        uint32_t modifiers = 0;
        input::MouseButton button = input::MouseButton::NONE;
        Vector2 difference;
        Vector2 position;
        Vector2 scroll;
    };

    struct TouchEvent
    {
        uint64_t touchId = 0;
        Vector2 difference;
        Vector2 position;
    };

    struct GamepadEvent
    {
        input::Gamepad* gamepad = nullptr;
        input::GamepadButton button = input::GamepadButton::NONE;
        bool pressed = false;
        float value = 0.0f;
    };

    struct WindowEvent
    {
        Window* window = nullptr;
        Size2 size;
        std::string title;
        bool fullscreen = false;
    };

    struct SystemEvent
    {
        enum class Orientation
        {
            UNKNOWN,
            PORTRAIT,
            PORTRAIT_UPSIDE_DOWN,
            LANDSCAPE_LEFT,
            LANDSCAPE_RIGHT,
            FACE_UP,
            FACE_DOWN
        };

        Orientation orientation;
        std::string filename;
    };

    struct UIEvent
    {
        scene::Node* node = nullptr;
        uint64_t touchId = 0;
        Vector2 position;
    };

    struct UserEvent
    {
        std::vector<std::string> parameters;
    };

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
            WINDOW_FULLSCREEN_CHANGE,
            WINDOW_RESOLUTION_CHANGE,
            ORIENTATION_CHANGE,
            LOW_MEMORY,
            OPEN_FILE,

            UI_ENTER_NODE, // mouse or touch entered the scene node
            UI_LEAVE_NODE, // mouse or touch left the scene node
            UI_PRESS_NODE, // mouse or touch pressed on scene node
            UI_RELEASE_NODE,  // mouse or touch released on scene node
            UI_CLICK_NODE, // mouse or touch clicked on scene node
            UI_DRAG_NODE, // mouse or touch dragged scene node
            UI_WIDGET_CHANGE, // widget changed its value

            USER // user defined event
        };

        Type type;

        KeyboardEvent keyboardEvent;
        MouseEvent mouseEvent;
        TouchEvent touchEvent;
        GamepadEvent gamepadEvent;
        WindowEvent windowEvent;
        SystemEvent systemEvent;
        UIEvent uiEvent;
        UserEvent userEvent;
    };
}
