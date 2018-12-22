// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_EVENTS_EVENT_HPP
#define OUZEL_EVENTS_EVENT_HPP

#include <cstdint>
#include <vector>
#include <string>
#include "audio/Voice.hpp"
#include "core/Window.hpp"
#include "math/Vector2.hpp"
#include "math/Size2.hpp"
#include "input/Gamepad.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include "input/Touchpad.hpp"

namespace ouzel
{
    struct Event
    {
        enum class Type
        {
            // gamepad events
            GAMEPAD_CONNECT,
            GAMEPAD_DISCONNECT,
            GAMEPAD_BUTTON_CHANGE,

            // keyboard events
            KEYBOARD_CONNECT,
            KEYBOARD_DISCONNECT,
            KEY_PRESS,
            KEY_RELEASE,

            // mouse events
            MOUSE_CONNECT,
            MOUSE_DISCONNECT,
            MOUSE_PRESS,
            MOUSE_RELEASE,
            MOUSE_SCROLL,
            MOUSE_MOVE,
            MOUSE_CURSOR_LOCK_CHANGE,

            // touch events
            TOUCHPAD_CONNECT,
            TOUCHPAD_DISCONNECT,
            TOUCH_BEGIN,
            TOUCH_MOVE,
            TOUCH_END,
            TOUCH_CANCEL,

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
            ACTOR_RELEASE, // mouse or touch released on scene actor
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

            UPDATE,

            USER // user defined event
        };

        Type type;
    };

    struct KeyboardEvent final: Event
    {
        input::Keyboard* keyboard = nullptr;
        input::Keyboard::Key key = input::Keyboard::Key::NONE;
    };

    struct MouseEvent final: Event
    {
        input::Mouse* mouse = nullptr;
        input::Mouse::Button button = input::Mouse::Button::NONE;
        Vector2 difference;
        Vector2 position;
        Vector2 scroll;
        bool locked = false;
    };

    struct TouchEvent final: Event
    {
        input::Touchpad* touchpad = nullptr;
        uint64_t touchId = 0;
        Vector2 difference;
        Vector2 position;
        float force = 1.0F;
    };

    struct GamepadEvent final: Event
    {
        input::Gamepad* gamepad = nullptr;
        input::Gamepad::Button button = input::Gamepad::Button::NONE;
        bool pressed = false;
        bool previousPressed = false;
        float value = 0.0F;
        float previousValue = 0.0F;
    };

    struct WindowEvent final: Event
    {
        Window* window = nullptr;
        Size2 size;
        std::string title;
        bool fullscreen = false;
        uint32_t screenId = 0;
    };

    struct SystemEvent final: Event
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

    namespace scene
    {
        class Actor;
        class Component;
    }

    struct UIEvent final: Event
    {
        scene::Actor* actor;
        uint64_t touchId = 0;
        Vector2 difference;
        Vector2 position;
        Vector3 localPosition;
    };

    struct AnimationEvent final: Event
    {
        scene::Component* component;
        std::string name;
    };

    struct SoundEvent final: Event
    {
        audio::Voice* voice;
    };

    struct UpdateEvent final: Event
    {
        float delta;
    };

    struct UserEvent final: Event
    {
        std::vector<std::string> parameters;
    };
}

#endif // OUZEL_EVENTS_EVENT_HPP
