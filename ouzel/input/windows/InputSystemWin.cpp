// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <unordered_map>
#include "InputSystemWin.hpp"
#include "events/Event.hpp"

namespace ouzel
{
    namespace input
    {
        static const std::unordered_map<UINT, Keyboard::Key> keyMap = {
            {VK_CANCEL, Keyboard::Key::CANCEL},
            {VK_BACK, Keyboard::Key::BACKSPACE},
            {VK_TAB, Keyboard::Key::TAB},
            {VK_CLEAR, Keyboard::Key::CLEAR},
            {VK_RETURN, Keyboard::Key::RETURN},

            {VK_MENU, Keyboard::Key::LEFT_ALT},
            {VK_PAUSE, Keyboard::Key::PAUSE},
            {VK_CAPITAL, Keyboard::Key::CAPITAL},

            {VK_ESCAPE, Keyboard::Key::ESCAPE},
            {VK_SPACE, Keyboard::Key::SPACE},
            {VK_PRIOR, Keyboard::Key::PRIOR},
            {VK_NEXT, Keyboard::Key::NEXT},
            {VK_END, Keyboard::Key::END},
            {VK_HOME, Keyboard::Key::HOME},
            {VK_LEFT, Keyboard::Key::LEFT},
            {VK_UP, Keyboard::Key::UP},
            {VK_RIGHT, Keyboard::Key::RIGHT},
            {VK_DOWN, Keyboard::Key::DOWN},

            {VK_SELECT, Keyboard::Key::SELECT},
            {VK_PRINT, Keyboard::Key::PRINT},
            {VK_EXECUTE, Keyboard::Key::EXECUTE},

            {VK_SNAPSHOT, Keyboard::Key::SNAPSHOT},
            {VK_INSERT, Keyboard::Key::INSERT},
            {VK_DELETE, Keyboard::Key::DEL},
            {VK_HELP, Keyboard::Key::HELP},

            {'0', Keyboard::Key::NUM_0},
            {'1', Keyboard::Key::NUM_1},
            {'2', Keyboard::Key::NUM_2},
            {'3', Keyboard::Key::NUM_3},
            {'4', Keyboard::Key::NUM_4},
            {'5', Keyboard::Key::NUM_5},
            {'6', Keyboard::Key::NUM_6},
            {'7', Keyboard::Key::NUM_7},
            {'8', Keyboard::Key::NUM_8},
            {'9', Keyboard::Key::NUM_9},

            {'A', Keyboard::Key::A},
            {'B', Keyboard::Key::B},
            {'C', Keyboard::Key::C},
            {'D', Keyboard::Key::D},
            {'E', Keyboard::Key::E},
            {'F', Keyboard::Key::F},
            {'G', Keyboard::Key::G},
            {'H', Keyboard::Key::H},
            {'I', Keyboard::Key::I},
            {'J', Keyboard::Key::J},
            {'K', Keyboard::Key::K},
            {'L', Keyboard::Key::L},
            {'M', Keyboard::Key::M},
            {'N', Keyboard::Key::N},
            {'O', Keyboard::Key::O},
            {'P', Keyboard::Key::P},
            {'Q', Keyboard::Key::Q},
            {'R', Keyboard::Key::R},
            {'S', Keyboard::Key::S},
            {'T', Keyboard::Key::T},
            {'U', Keyboard::Key::U},
            {'V', Keyboard::Key::V},
            {'W', Keyboard::Key::W},
            {'X', Keyboard::Key::X},
            {'Y', Keyboard::Key::Y},
            {'Z', Keyboard::Key::Z},

            {VK_LWIN, Keyboard::Key::LEFT_SUPER},
            {VK_RWIN, Keyboard::Key::RIGHT_SUPER},
            {VK_APPS, Keyboard::Key::MENU},
            {VK_SLEEP, Keyboard::Key::SLEEP},

            {VK_NUMPAD0, Keyboard::Key::NUMPAD_0},
            {VK_NUMPAD1, Keyboard::Key::NUMPAD_1},
            {VK_NUMPAD2, Keyboard::Key::NUMPAD_2},
            {VK_NUMPAD3, Keyboard::Key::NUMPAD_3},
            {VK_NUMPAD4, Keyboard::Key::NUMPAD_4},
            {VK_NUMPAD5, Keyboard::Key::NUMPAD_5},
            {VK_NUMPAD6, Keyboard::Key::NUMPAD_6},
            {VK_NUMPAD7, Keyboard::Key::NUMPAD_7},
            {VK_NUMPAD8, Keyboard::Key::NUMPAD_8},
            {VK_NUMPAD9, Keyboard::Key::NUMPAD_9},

            {VK_MULTIPLY, Keyboard::Key::MULTIPLY},
            {VK_ADD, Keyboard::Key::ADD},
            {VK_SEPARATOR, Keyboard::Key::SEPARATOR},
            {VK_SUBTRACT, Keyboard::Key::SUBTRACT},
            {VK_DECIMAL, Keyboard::Key::DECIMAL},
            {VK_DIVIDE, Keyboard::Key::DIVIDE},

            {VK_F1, Keyboard::Key::F1},
            {VK_F2, Keyboard::Key::F2},
            {VK_F3, Keyboard::Key::F3},
            {VK_F4, Keyboard::Key::F4},
            {VK_F5, Keyboard::Key::F5},
            {VK_F6, Keyboard::Key::F6},
            {VK_F7, Keyboard::Key::F7},
            {VK_F8, Keyboard::Key::F8},
            {VK_F9, Keyboard::Key::F9},
            {VK_F10, Keyboard::Key::F10},
            {VK_F11, Keyboard::Key::F11},
            {VK_F12, Keyboard::Key::F12},
            {VK_F13, Keyboard::Key::F13},
            {VK_F14, Keyboard::Key::F14},
            {VK_F15, Keyboard::Key::F15},
            {VK_F16, Keyboard::Key::F16},
            {VK_F17, Keyboard::Key::F17},
            {VK_F18, Keyboard::Key::F18},
            {VK_F19, Keyboard::Key::F19},
            {VK_F20, Keyboard::Key::F20},
            {VK_F21, Keyboard::Key::F21},
            {VK_F22, Keyboard::Key::F22},
            {VK_F23, Keyboard::Key::F23},
            {VK_F24, Keyboard::Key::F24},

            {VK_NUMLOCK, Keyboard::Key::NUMLOCK},
            {VK_SCROLL, Keyboard::Key::SCROLL},
            {VK_LSHIFT, Keyboard::Key::LEFT_SHIFT},
            {VK_RSHIFT, Keyboard::Key::RIGHT_SHIFT},
            {VK_LCONTROL, Keyboard::Key::LEFT_CONTROL},
            {VK_RCONTROL, Keyboard::Key::RIGHT_CONTROL},
            {VK_LMENU, Keyboard::Key::LEFT_ALT},
            {VK_RMENU, Keyboard::Key::RIGHT_ALT},

            {VK_OEM_1, Keyboard::Key::SEMICOLON},
            {VK_OEM_PLUS, Keyboard::Key::PLUS},
            {VK_OEM_COMMA, Keyboard::Key::COMMA},
            {VK_OEM_MINUS, Keyboard::Key::MINUS},
            {VK_OEM_PERIOD, Keyboard::Key::PERIOD},
            {VK_OEM_2, Keyboard::Key::SLASH},
            {VK_OEM_3, Keyboard::Key::GRAVE},
            {VK_OEM_4, Keyboard::Key::LEFT_BRACKET},
            {VK_OEM_5, Keyboard::Key::BACKSLASH},
            {VK_OEM_6, Keyboard::Key::RIGHT_BRACKET},
            {VK_OEM_7, Keyboard::Key::QUOTE},
            {VK_OEM_8, Keyboard::Key::GRAVE},
            {VK_OEM_AX, Keyboard::Key::OEM_AX},
            {VK_OEM_102, Keyboard::Key::LESS}
        };

        Keyboard::Key InputSystemWin::convertKeyCode(UINT keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
        }

        uint32_t InputSystemWin::getModifiers(WPARAM wParam)
        {
            uint32_t modifiers = 0;

            if (wParam & MK_SHIFT) modifiers |= SHIFT_DOWN;
            if (GetKeyState(VK_MENU) & 0x8000) modifiers |= ALT_DOWN;
            if (wParam & MK_CONTROL) modifiers |= CONTROL_DOWN;
            if (wParam & MK_LBUTTON) modifiers |= LEFT_MOUSE_DOWN;
            if (wParam & MK_RBUTTON) modifiers |= RIGHT_MOUSE_DOWN;
            if (wParam & MK_MBUTTON) modifiers |= MIDDLE_MOUSE_DOWN;

            return modifiers;
        }

        void InputSystemWin::handleButtonValueChange(const GamepadDeviceWin& gamepad, Gamepad::Button button, bool pressed, float value)
        {
            Event event;
            event.type = Event::Type::GAMEPAD_BUTTON_CHANGE;

            event.deviceId = gamepad.getDeviceId();
            event.gamepadButton = button;
            event.pressed = pressed;
            event.value = value;

            addEvent(event);
        }
    } // namespace input
} // namespace ouzel
