// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_EVENTS_EVENT_HPP
#define OUZEL_EVENTS_EVENT_HPP

#include <cstdint>
#include <vector>
#include <string>
#include "../audio/Voice.hpp"
#include "../core/Window.hpp"
#include "../math/Vector.hpp"
#include "../math/Size.hpp"
#include "../input/Gamepad.hpp"
#include "../input/Keyboard.hpp"
#include "../input/Mouse.hpp"
#include "../input/Touchpad.hpp"

namespace ouzel
{
    struct Event
    {
        enum class Type
        {
            // gamepad events
            GamepadConnect,
            GamepadDisconnect,
            GamepadButtonChange,

            // keyboard events
            KeyboardConnect,
            KeyboardDisconnect,
            KeyboardKeyPress,
            KeyboardKeyRelease,

            // mouse events
            MouseConnect,
            MouseDisconnect,
            MousePress,
            MouseRelease,
            MouseScroll,
            MouseMove,
            MouseCursorLockChange,

            // touch events
            TouchpadConnect,
            TouchpadDisconnect,
            TouchBegin,
            TouchMove,
            TouchEnd,
            TouchCancel,

            // window events
            WindowSizeChange,
            WindowTitleChange,
            FullscreenChange,
            ScreenChange,
            ResolutionChange,

            // system events
            EngineStart, // engine started running (sent only once)
            EngineStop, // engine stopped running (sent only once)
            EngineResume, // engine resumed running
            EnginePause, // engine paused running
            OrientationChange,
            LowMemory,
            OpenFile,

            // UI events
            ActorEnter, // mouse or touch entered the scene actor
            ActorLeave, // mouse or touch left the scene actor
            ActorPress, // mouse or touch pressed on scene actor
            ActorRelease, // mouse or touch released on scene actor
            ActorClick, // mouse or touch clicked on scene actor
            ActorDrag, // mouse or touch dragged scene actor
            WidgetChange, // widget changed its value

            // animation events
            AnimationStart, // animation has started
            AnimationReset, // animation has reset
            AnimationFinish, // animation has finished

            // sound events
            SoundStart,
            SoundReset,
            SoundFinish,

            Update,

            User // user defined event
        };

        Type type;
    };

    struct KeyboardEvent final: Event
    {
        input::Keyboard* keyboard = nullptr;
        input::Keyboard::Key key = input::Keyboard::Key::Unknown;
    };

    struct MouseEvent final: Event
    {
        input::Mouse* mouse = nullptr;
        input::Mouse::Button button = input::Mouse::Button::Unknown;
        Vector2F difference;
        Vector2F position;
        Vector2F scroll;
        bool locked = false;
    };

    struct TouchEvent final: Event
    {
        input::Touchpad* touchpad = nullptr;
        std::uint64_t touchId = 0;
        Vector2F difference;
        Vector2F position;
        float force = 1.0F;
    };

    struct GamepadEvent final: Event
    {
        input::Gamepad* gamepad = nullptr;
        input::Gamepad::Button button = input::Gamepad::Button::Unknown;
        bool pressed = false;
        bool previousPressed = false;
        float value = 0.0F;
        float previousValue = 0.0F;
    };

    struct WindowEvent final: Event
    {
        Window* window = nullptr;
        Size2U size;
        std::string title;
        bool fullscreen = false;
        std::uint32_t screenId = 0;
    };

    struct SystemEvent final: Event
    {
        enum class Orientation
        {
            Portrait,
            PortraitReverse,
            Landscape,
            LandscapeReverse,
            FaceUp,
            FaceDown
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
        std::uint64_t touchId = 0;
        Vector2F difference;
        Vector2F position;
        Vector3F localPosition;
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
