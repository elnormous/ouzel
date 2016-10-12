// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <emscripten.h>
#include "InputEm.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "events/Event.h"
#include "utils/Log.h"

EM_BOOL emKeyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
    switch (eventType)
    {
        case EMSCRIPTEN_EVENT_KEYPRESS:
        case EMSCRIPTEN_EVENT_KEYDOWN:
            ouzel::sharedEngine->getInput()->keyDown(ouzel::input::InputEm::convertKeyCode(keyEvent->code),
                                                     ouzel::input::InputEm::getKeyboardModifiers(keyEvent));
            return true;
        case EMSCRIPTEN_EVENT_KEYUP:
            ouzel::sharedEngine->getInput()->keyUp(ouzel::input::InputEm::convertKeyCode(keyEvent->code),
                                                   ouzel::input::InputEm::getKeyboardModifiers(keyEvent));
            return true;
    }

    return false;
}

EM_BOOL emMouseCallback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
    ouzel::input::MouseButton button;

    switch (mouseEvent->button)
    {
        case 0:
            button = ouzel::input::MouseButton::LEFT;
            break;
        case 1:
            button = ouzel::input::MouseButton::RIGHT;
            break;
        case 2:
            button = ouzel::input::MouseButton::MIDDLE;
            break;
        default:
            button = ouzel::input::MouseButton::NONE;
            break;
    }

    ouzel::Vector2 position(static_cast<float>(mouseEvent->clientX),
                            static_cast<float>(mouseEvent->clientY));

    switch (eventType)
    {
        case EMSCRIPTEN_EVENT_MOUSEDOWN:
            ouzel::sharedEngine->getInput()->mouseDown(button,
                                                       ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(position),
                                                       ouzel::input::InputEm::getMouseModifiers(mouseEvent));
            return true;
        case EMSCRIPTEN_EVENT_MOUSEUP:
            ouzel::sharedEngine->getInput()->mouseUp(button,
                                                     ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(position),
                                                     ouzel::input::InputEm::getMouseModifiers(mouseEvent));
            return true;
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
            ouzel::sharedEngine->getInput()->mouseMove(ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(position),
                                                       ouzel::input::InputEm::getMouseModifiers(mouseEvent));
            return true;
    }

    return false;
}

EM_BOOL emWheelCallback(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData)
{
    if (eventType == EMSCRIPTEN_EVENT_WHEEL)
    {
        ouzel::Vector2 position(static_cast<float>(wheelEvent->mouse.clientX),
                                static_cast<float>(wheelEvent->mouse.clientY));

        ouzel::sharedEngine->getInput()->mouseScroll(ouzel::Vector2(static_cast<float>(wheelEvent->deltaX), static_cast<float>(wheelEvent->deltaY)),
                                                     ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(position),
                                                     ouzel::input::InputEm::getMouseModifiers(&wheelEvent->mouse));

        return true;
    }

    return false;
}

namespace ouzel
{
    namespace input
    {
        static std::map<std::string, KeyboardKey> keyMap = {
            { "Backspace", KeyboardKey::BACKSPACE },
            { "Tab", KeyboardKey::TAB },
            { "Enter", KeyboardKey::RETURN },
            { "Shift", KeyboardKey::SHIFT },
            { "ControlLeft", KeyboardKey::LCONTROL },
            { "ControlRight", KeyboardKey::RCONTROL },
            { "AltLeft", KeyboardKey::LALT },
            { "AltRight", KeyboardKey::RALT },
            { "Pause", KeyboardKey::PAUSE },
            { "CapsLock", KeyboardKey::CAPITAL },
            { "Escape", KeyboardKey::ESCAPE },
            { "PageUp", KeyboardKey::PRIOR },
            { "PageDown", KeyboardKey::NEXT },
            { "Home", KeyboardKey::HOME },
            { "End", KeyboardKey::END },
            { "ArrowLeft", KeyboardKey::LEFT },
            { "ArrowUp", KeyboardKey::UP },
            { "ArrowRight", KeyboardKey::RIGHT },
            { "ArrowDown", KeyboardKey::DOWN },
            { "Insert", KeyboardKey::INSERT },
            { "Delete", KeyboardKey::DEL },
            { "Digit0", KeyboardKey::KEY_0 },
            { "Digit1", KeyboardKey::KEY_1 },
            { "Digit2", KeyboardKey::KEY_2 },
            { "Digit3", KeyboardKey::KEY_3 },
            { "Digit4", KeyboardKey::KEY_4 },
            { "Digit5", KeyboardKey::KEY_5 },
            { "Digit6", KeyboardKey::KEY_6 },
            { "Digit7", KeyboardKey::KEY_7 },
            { "Digit8", KeyboardKey::KEY_8 },
            { "Digit9", KeyboardKey::KEY_9 },
            { "KeyA", KeyboardKey::KEY_A },
            { "KeyB", KeyboardKey::KEY_B },
            { "KeyC", KeyboardKey::KEY_C },
            { "KeyD", KeyboardKey::KEY_D },
            { "KeyE", KeyboardKey::KEY_E },
            { "KeyF", KeyboardKey::KEY_F },
            { "KeyG", KeyboardKey::KEY_G },
            { "KeyH", KeyboardKey::KEY_H },
            { "KeyI", KeyboardKey::KEY_I },
            { "KeyJ", KeyboardKey::KEY_J },
            { "KeyK", KeyboardKey::KEY_K },
            { "KeyL", KeyboardKey::KEY_L },
            { "KeyM", KeyboardKey::KEY_M },
            { "KeyN", KeyboardKey::KEY_N },
            { "KeyO", KeyboardKey::KEY_O },
            { "KeyP", KeyboardKey::KEY_P },
            { "KeyQ", KeyboardKey::KEY_Q },
            { "KeyR", KeyboardKey::KEY_R },
            { "KeyS", KeyboardKey::KEY_S },
            { "KeyT", KeyboardKey::KEY_T },
            { "KeyU", KeyboardKey::KEY_U },
            { "KeyV", KeyboardKey::KEY_V },
            { "KeyW", KeyboardKey::KEY_W },
            { "KeyX", KeyboardKey::KEY_X },
            { "KeyY", KeyboardKey::KEY_Y },
            { "KeyZ", KeyboardKey::KEY_Z },
            { "OSLeft", KeyboardKey::LSUPER },
            { "OSRight", KeyboardKey::RSUPER },
            { "Delete", KeyboardKey::DEL },
            { "NumpadEnter", KeyboardKey::SEPARATOR },
            { "NumpadDigit0", KeyboardKey::NUMPAD0 },
            { "NumpadDigit1", KeyboardKey::NUMPAD1 },
            { "NumpadDigit2", KeyboardKey::NUMPAD2 },
            { "NumpadDigit3", KeyboardKey::NUMPAD3 },
            { "NumpadDigit4", KeyboardKey::NUMPAD4 },
            { "NumpadDigit5", KeyboardKey::NUMPAD5 },
            { "NumpadDigit6", KeyboardKey::NUMPAD6 },
            { "NumpadDigit7", KeyboardKey::NUMPAD7 },
            { "NumpadDigit8", KeyboardKey::NUMPAD8 },
            { "NumpadDigit9", KeyboardKey::NUMPAD9 },
            { "NumpadMultiply", KeyboardKey::MULTIPLY },
            { "NumpadAdd", KeyboardKey::ADD },
            { "NumpadSubtract", KeyboardKey::SUBTRACT },
            { "NumpadDecimal", KeyboardKey::DECIMAL },
            { "NumpadDivide", KeyboardKey::DIVIDE },
            { "F1", KeyboardKey::F1 },
            { "F2", KeyboardKey::F2 },
            { "F3", KeyboardKey::F3 },
            { "F4", KeyboardKey::F4 },
            { "F5", KeyboardKey::F5 },
            { "F6", KeyboardKey::F6 },
            { "F7", KeyboardKey::F7 },
            { "F8", KeyboardKey::F8 },
            { "F9", KeyboardKey::F9 },
            { "F10", KeyboardKey::F10 },
            { "F11", KeyboardKey::F11 },
            { "F12", KeyboardKey::F12 },
            { "NumLock", KeyboardKey::NUMLOCK },
            { "ScrollLock", KeyboardKey::SCROLL },
            { "Semicolon", KeyboardKey::SEMICOLON },
            { "Equal", KeyboardKey::EQUAL },
            { "Comma", KeyboardKey::COMMA },
            { "Minus", KeyboardKey::MINUS },
            { "Period", KeyboardKey::PERIOD },
            { "Slash", KeyboardKey::SLASH },
            { "Backquote", KeyboardKey::GRAVE },
            { "BracketLeft", KeyboardKey::BRACKET_LEFT },
            { "Backslash", KeyboardKey::BACKSLASH },
            { "BracketRight", KeyboardKey::BRACKET_RIGHT },
            { "Quote", KeyboardKey::BRACKET_RIGHT },
            { "IntlBackslash", KeyboardKey::LESS }
        };

        InputEm::InputEm()
        {
            emscripten_set_keypress_callback(nullptr, this, 1, emKeyCallback);
            emscripten_set_keydown_callback(nullptr, this, 1, emKeyCallback);
            emscripten_set_keyup_callback(nullptr, this, 1, emKeyCallback);

            emscripten_set_mousedown_callback("#canvas", this, 1, emMouseCallback);
            emscripten_set_mouseup_callback("#canvas", this, 1, emMouseCallback);
            emscripten_set_mousemove_callback("#canvas", this, 1, emMouseCallback);

            emscripten_set_wheel_callback("#canvas", this, 1, emWheelCallback);
        }

        InputEm::~InputEm()
        {
        }

        KeyboardKey InputEm::convertKeyCode(const EM_UTF8 key[32])
        {
            auto i = keyMap.find(key);

            if (i != keyMap.end())
            {
                return i->second;
            }
            else
            {
                return KeyboardKey::NONE;
            }
        }

        uint32_t InputEm::getKeyboardModifiers(const EmscriptenKeyboardEvent* keyboardEvent)
        {
            uint32_t modifiers = 0;

            if (keyboardEvent->ctrlKey) modifiers |= ouzel::CONTROL_DOWN;
            if (keyboardEvent->shiftKey) modifiers |= ouzel::SHIFT_DOWN;
            if (keyboardEvent->altKey) modifiers |= ouzel::ALT_DOWN;
            if (keyboardEvent->metaKey) modifiers |= ouzel::SUPER_DOWN;

            return modifiers;
        }

        uint32_t InputEm::getMouseModifiers(const EmscriptenMouseEvent* mouseEvent)
        {
            uint32_t modifiers = 0;

            if (mouseEvent->ctrlKey) modifiers |= ouzel::CONTROL_DOWN;
            if (mouseEvent->shiftKey) modifiers |= ouzel::SHIFT_DOWN;
            if (mouseEvent->altKey) modifiers |= ouzel::ALT_DOWN;
            if (mouseEvent->metaKey) modifiers |= ouzel::SUPER_DOWN;

            return modifiers;
        }

        void InputEm::setCursorVisible(bool visible)
        {
            if (!visible)
            {
                cursorVisible = visible;
                emscripten_hide_mouse();
            }
            else
            {
                Log(Log::Level::WARN) << "Cursors showing is not implemented for Emscripten target";
            }
        }

        bool InputEm::isCursorVisible() const
        {
            return cursorVisible;
        }
    } // namespace input
} // namespace ouzel
