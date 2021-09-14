// Ouzel by Elviss Strazdins

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
            gamepadConnect,
            gamepadDisconnect,
            gamepadButtonChange,

            // keyboard events
            keyboardConnect,
            keyboardDisconnect,
            keyboardKeyPress,
            keyboardKeyRelease,

            // mouse events
            mouseConnect,
            mouseDisconnect,
            mousePress,
            mouseRelease,
            mouseScroll,
            mouseMove,
            mouseCursorLockChange,

            // touch events
            touchpadConnect,
            touchpadDisconnect,
            touchBegin,
            touchMove,
            touchEnd,
            touchCancel,

            // window events
            windowSizeChange,
            windowTitleChange,
            fullscreenChange,
            screenChange,
            resolutionChange,

            // system events
            engineStart, // engine started running (sent only once)
            engineStop, // engine stopped running (sent only once)
            engineResume, // engine resumed running
            enginePause, // engine paused running
            orientationChange,
            lowMemory,
            openFile,

            // UI events
            actorEnter, // mouse or touch entered the scene actor
            actorLeave, // mouse or touch left the scene actor
            actorPress, // mouse or touch pressed on scene actor
            actorRelease, // mouse or touch released on scene actor
            actorClick, // mouse or touch clicked on scene actor
            actorDrag, // mouse or touch dragged scene actor
            widgetChange, // widget changed its value

            // animation events
            animationStart, // animation has started
            animationReset, // animation has reset
            animationFinish, // animation has finished

            // sound events
            soundStart,
            soundReset,
            soundFinish,

            update,

            user // user defined event
        };

        Type type;
    };

    struct KeyboardEvent final: Event
    {
        input::Keyboard* keyboard = nullptr;
        input::Keyboard::Key key = input::Keyboard::Key::none;
    };

    struct MouseEvent final: Event
    {
        input::Mouse* mouse = nullptr;
        input::Mouse::Button button = input::Mouse::Button::none;
        math::Vector<float, 2> difference{};
        math::Vector<float, 2> position{};
        math::Vector<float, 2> scroll{};
        bool locked = false;
    };

    struct TouchEvent final: Event
    {
        input::Touchpad* touchpad = nullptr;
        std::uint64_t touchId = 0;
        math::Vector<float, 2> difference{};
        math::Vector<float, 2> position{};
        float force = 1.0F;
    };

    struct GamepadEvent final: Event
    {
        input::Gamepad* gamepad = nullptr;
        input::Gamepad::Button button = input::Gamepad::Button::none;
        bool pressed = false;
        bool previousPressed = false;
        float value = 0.0F;
        float previousValue = 0.0F;
    };

    struct WindowEvent final: Event
    {
        core::Window* window = nullptr;
        math::Size<std::uint32_t, 2> size;
        std::string title;
        bool fullscreen = false;
        std::uint32_t screenId = 0;
    };

    struct SystemEvent final: Event
    {
        enum class Orientation
        {
            portrait,
            portraitReverse,
            landscape,
            landscapeReverse,
            faceUp,
            faceDown
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
        scene::Actor* actor = nullptr;
        std::uint64_t touchId = 0;
        math::Vector<float, 2> difference{};
        math::Vector<float, 2> position{};
        math::Vector<float, 3> localPosition{};
    };

    struct AnimationEvent final: Event
    {
        scene::Component* component = nullptr;
        std::string name;
    };

    struct SoundEvent final: Event
    {
        audio::Voice* voice = nullptr;
    };

    struct UpdateEvent final: Event
    {
        float delta = 0.0F;
    };

    struct UserEvent final: Event
    {
        std::vector<std::string> parameters;
    };
}

#endif // OUZEL_EVENTS_EVENT_HPP
