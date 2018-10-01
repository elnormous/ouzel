// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <map>
#include <emscripten.h>
#include "InputSystemEm.hpp"
#include "GamepadDeviceEm.hpp"
#include "events/Event.hpp"

namespace ouzel
{
    namespace input
    {
        static std::map<std::string, Keyboard::Key> keyMap = {
            {"Backspace", Keyboard::Key::BACKSPACE},
            {"Tab", Keyboard::Key::TAB},
            {"Enter", Keyboard::Key::RETURN},
            {"ShiftLeft", Keyboard::Key::LEFT_SHIFT},
            {"ShiftRight", Keyboard::Key::RIGHT_SHIFT},
            {"ControlLeft", Keyboard::Key::LEFT_CONTROL},
            {"ControlRight", Keyboard::Key::RIGHT_CONTROL},
            {"AltLeft", Keyboard::Key::LEFT_ALT},
            {"AltRight", Keyboard::Key::RIGHT_ALT},
            {"MetaLeft", Keyboard::Key::LEFT_SUPER},
            {"MetaRight", Keyboard::Key::RIGHT_SUPER},
            {"Pause", Keyboard::Key::PAUSE},
            {"CapsLock", Keyboard::Key::CAPITAL},
            {"Escape", Keyboard::Key::ESCAPE},
            {"PageUp", Keyboard::Key::PRIOR},
            {"PageDown", Keyboard::Key::NEXT},
            {"Home", Keyboard::Key::HOME},
            {"End", Keyboard::Key::END},
            {"ArrowLeft", Keyboard::Key::LEFT},
            {"ArrowUp", Keyboard::Key::UP},
            {"ArrowRight", Keyboard::Key::RIGHT},
            {"ArrowDown", Keyboard::Key::DOWN},
            {"Insert", Keyboard::Key::INSERT},
            {"Delete", Keyboard::Key::DEL},
            {"Digit0", Keyboard::Key::NUM_0},
            {"Digit1", Keyboard::Key::NUM_1},
            {"Digit2", Keyboard::Key::NUM_2},
            {"Digit3", Keyboard::Key::NUM_3},
            {"Digit4", Keyboard::Key::NUM_4},
            {"Digit5", Keyboard::Key::NUM_5},
            {"Digit6", Keyboard::Key::NUM_6},
            {"Digit7", Keyboard::Key::NUM_7},
            {"Digit8", Keyboard::Key::NUM_8},
            {"Digit9", Keyboard::Key::NUM_9},
            {"KeyA", Keyboard::Key::A},
            {"KeyB", Keyboard::Key::B},
            {"KeyC", Keyboard::Key::C},
            {"KeyD", Keyboard::Key::D},
            {"KeyE", Keyboard::Key::E},
            {"KeyF", Keyboard::Key::F},
            {"KeyG", Keyboard::Key::G},
            {"KeyH", Keyboard::Key::H},
            {"KeyI", Keyboard::Key::I},
            {"KeyJ", Keyboard::Key::J},
            {"KeyK", Keyboard::Key::K},
            {"KeyL", Keyboard::Key::L},
            {"KeyM", Keyboard::Key::M},
            {"KeyN", Keyboard::Key::N},
            {"KeyO", Keyboard::Key::O},
            {"KeyP", Keyboard::Key::P},
            {"KeyQ", Keyboard::Key::Q},
            {"KeyR", Keyboard::Key::R},
            {"KeyS", Keyboard::Key::S},
            {"KeyT", Keyboard::Key::T},
            {"KeyU", Keyboard::Key::U},
            {"KeyV", Keyboard::Key::V},
            {"KeyW", Keyboard::Key::W},
            {"KeyX", Keyboard::Key::X},
            {"KeyY", Keyboard::Key::Y},
            {"KeyZ", Keyboard::Key::Z},
            {"OSLeft", Keyboard::Key::LEFT_SUPER},
            {"OSRight", Keyboard::Key::RIGHT_SUPER},
            {"Delete", Keyboard::Key::DEL},
            {"NumpadEnter", Keyboard::Key::SEPARATOR},
            {"NumpadDigit0", Keyboard::Key::NUMPAD_0},
            {"NumpadDigit1", Keyboard::Key::NUMPAD_1},
            {"NumpadDigit2", Keyboard::Key::NUMPAD_2},
            {"NumpadDigit3", Keyboard::Key::NUMPAD_3},
            {"NumpadDigit4", Keyboard::Key::NUMPAD_4},
            {"NumpadDigit5", Keyboard::Key::NUMPAD_5},
            {"NumpadDigit6", Keyboard::Key::NUMPAD_6},
            {"NumpadDigit7", Keyboard::Key::NUMPAD_7},
            {"NumpadDigit8", Keyboard::Key::NUMPAD_8},
            {"NumpadDigit9", Keyboard::Key::NUMPAD_9},
            {"NumpadMultiply", Keyboard::Key::MULTIPLY},
            {"NumpadAdd", Keyboard::Key::ADD},
            {"NumpadSubtract", Keyboard::Key::SUBTRACT},
            {"NumpadDecimal", Keyboard::Key::DECIMAL},
            {"NumpadDivide", Keyboard::Key::DIVIDE},
            {"F1", Keyboard::Key::F1},
            {"F2", Keyboard::Key::F2},
            {"F3", Keyboard::Key::F3},
            {"F4", Keyboard::Key::F4},
            {"F5", Keyboard::Key::F5},
            {"F6", Keyboard::Key::F6},
            {"F7", Keyboard::Key::F7},
            {"F8", Keyboard::Key::F8},
            {"F9", Keyboard::Key::F9},
            {"F10", Keyboard::Key::F10},
            {"F11", Keyboard::Key::F11},
            {"F12", Keyboard::Key::F12},
            {"NumLock", Keyboard::Key::NUMLOCK},
            {"ScrollLock", Keyboard::Key::SCROLL},
            {"Semicolon", Keyboard::Key::SEMICOLON},
            {"Equal", Keyboard::Key::EQUAL},
            {"Comma", Keyboard::Key::COMMA},
            {"Minus", Keyboard::Key::MINUS},
            {"Period", Keyboard::Key::PERIOD},
            {"Slash", Keyboard::Key::SLASH},
            {"Backquote", Keyboard::Key::GRAVE},
            {"BracketLeft", Keyboard::Key::LEFT_BRACKET},
            {"Backslash", Keyboard::Key::BACKSLASH},
            {"BracketRight", Keyboard::Key::RIGHT_BRACKET},
            {"Quote", Keyboard::Key::QUOTE},
            {"IntlBackslash", Keyboard::Key::LESS}
        };

        Keyboard::Key InputSystemEm::convertKeyCode(const EM_UTF8 key[32])
        {
            auto i = keyMap.find(key);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
        }

        uint32_t InputSystemEm::getKeyboardModifiers(const EmscriptenKeyboardEvent* keyboardEvent)
        {
            uint32_t modifiers = 0;

            if (keyboardEvent->ctrlKey) modifiers |= ouzel::CONTROL_DOWN;
            if (keyboardEvent->shiftKey) modifiers |= ouzel::SHIFT_DOWN;
            if (keyboardEvent->altKey) modifiers |= ouzel::ALT_DOWN;
            if (keyboardEvent->metaKey) modifiers |= ouzel::SUPER_DOWN;

            return modifiers;
        }

        uint32_t InputSystemEm::getMouseModifiers(const EmscriptenMouseEvent* mouseEvent)
        {
            uint32_t modifiers = 0;

            if (mouseEvent->ctrlKey) modifiers |= ouzel::CONTROL_DOWN;
            if (mouseEvent->shiftKey) modifiers |= ouzel::SHIFT_DOWN;
            if (mouseEvent->altKey) modifiers |= ouzel::ALT_DOWN;
            if (mouseEvent->metaKey) modifiers |= ouzel::SUPER_DOWN;

            if (mouseEvent->buttons & (1 << 0)) modifiers |= LEFT_MOUSE_DOWN;
            if (mouseEvent->buttons & (1 << 1)) modifiers |= RIGHT_MOUSE_DOWN;
            if (mouseEvent->buttons & (1 << 2)) modifiers |= MIDDLE_MOUSE_DOWN;

            return modifiers;
        }
    } // namespace input
} // namespace ouzel
