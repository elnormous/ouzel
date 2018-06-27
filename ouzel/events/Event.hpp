// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "audio/Sound.hpp"
#include "scene/Actor.hpp"
#include "math/Vector2.hpp"
#include "math/Size2.hpp"
#include "input/InputManager.hpp"
#include "input/Gamepad.hpp"
#include "input/InputDevice.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include "input/Touchpad.hpp"

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
        input::Keyboard* keyboard = nullptr;
        uint32_t modifiers = 0;
        input::KeyboardKey key = input::KeyboardKey::NONE;
    };

    struct MouseEvent
    {
        input::Mouse* mouse = nullptr;
        uint32_t modifiers = 0;
        input::MouseButton button = input::MouseButton::NONE;
        Vector2 difference;
        Vector2 position;
        Vector2 scroll;
    };

    struct TouchEvent
    {
        input::Touchpad* touchpad = nullptr;
        uint64_t touchId = 0;
        Vector2 difference;
        Vector2 position;
        float force = 1.0F;
    };

    struct GamepadEvent
    {
        input::Gamepad* gamepad = nullptr;
        input::GamepadButton button = input::GamepadButton::NONE;
        bool pressed = false;
        bool previousPressed = false;
        float value = 0.0F;
        float previousValue = 0.0F;
    };

    class Window;

    struct WindowEvent
    {
        Window* window = nullptr;
        Size2 size;
        std::string title;
        bool fullscreen = false;
        uint32_t screenId = 0;
    };

    struct SystemEvent
    {
        enum class Orientation
        {
            UNKNOWN,
            PORTRAIT,
            PORTRAIT_REVERSE,
            LANDSCAPE,
            LANDSCAPE_REVERSE,
            FACE_UP,
            FACE_DOWN
        };

        Orientation orientation;
        std::string filename;
    };

    struct UIEvent
    {
        scene::Actor* actor;
        uint64_t touchId = 0;
        Vector2 difference;
        Vector2 position;
        Vector3 localPosition;
    };

    struct AnimationEvent
    {
        scene::Component* component;
        std::string name;
    };

    struct SoundEvent
    {
        audio::Sound* sound;
    };

    struct UserEvent
    {
        std::vector<std::string> parameters;
    };

    struct Event
    {
        enum class Type
        {
            // keyboard events
            KEY_PRESS,
            KEY_RELEASE,
            KEY_REPEAT,

            // mouse events
            MOUSE_PRESS,
            MOUSE_RELEASE,
            MOUSE_SCROLL,
            MOUSE_MOVE,

            // touch events
            TOUCH_BEGIN,
            TOUCH_MOVE,
            TOUCH_END,
            TOUCH_CANCEL,

            // gamepad events
            GAMEPAD_CONNECT,
            GAMEPAD_DISCONNECT,
            GAMEPAD_BUTTON_CHANGE,

            // window events
            WINDOW_SIZE_CHANGE,
            WINDOW_TITLE_CHANGE,
            FULLSCREEN_CHANGE,
            SCREEN_CHANGE,
            RESOLUTION_CHANGE,

            // system events
            ENGINE_START, // engine started running (sent only once)
            ENGINE_STOP, // engine stopped running (sent only once)
            ENGINE_RESUME, // engine resumed running
            ENGINE_PAUSE, // engine paused running
            ORIENTATION_CHANGE,
            LOW_MEMORY,
            OPEN_FILE,

            // UI events
            ACTOR_ENTER, // mouse or touch entered the scene actor
            ACTOR_LEAVE, // mouse or touch left the scene actor
            ACTOR_PRESS, // mouse or touch pressed on scene actor
            ACTOR_RELEASE,  // mouse or touch released on scene actor
            ACTOR_CLICK, // mouse or touch clicked on scene actor
            ACTOR_DRAG, // mouse or touch dragged scene actor
            WIDGET_CHANGE, // widget changed its value

            // animation events
            ANIMATION_START, // animation has started
            ANIMATION_RESET, // animation has reset
            ANIMATION_FINISH, // animation has finished

            // sound events
            SOUND_START,
            SOUND_RESET,
            SOUND_FINISH,

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
        AnimationEvent animationEvent;
        SoundEvent soundEvent;
        UserEvent userEvent;
    };
}
