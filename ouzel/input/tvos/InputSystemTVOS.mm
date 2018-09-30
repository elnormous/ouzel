// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <unordered_map>
#import <UIKit/UIPress.h>
#include "InputSystemTVOS.hpp"
#include "GamepadDeviceTVOS.hpp"

namespace ouzel
{
    namespace input
    {
        static const std::unordered_map<NSInteger, Keyboard::Key> keyMap = {
            {UIPressTypeUpArrow, Keyboard::Key::UP},
            {UIPressTypeDownArrow, Keyboard::Key::DOWN},
            {UIPressTypeLeftArrow, Keyboard::Key::LEFT},
            {UIPressTypeRightArrow, Keyboard::Key::RIGHT},
            {UIPressTypeSelect, Keyboard::Key::SELECT},
            {UIPressTypeMenu, Keyboard::Key::MENU},
            {UIPressTypePlayPause, Keyboard::Key::PAUSE}
        };

        Keyboard::Key InputSystemTVOS::convertKeyCode(NSInteger keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
        }

        void InputSystemTVOS::handleButtonValueChange(const GamepadDeviceTVOS& gamepad, Gamepad::Button button, bool pressed, float value)
        {
            Event event;
            event.type = Event::Type::GAMEPAD_BUTTON_CHANGE;

            event.deviceId = gamepad.getId();
            event.gamepadButton = button;
            event.pressed = pressed;
            event.value = value;

            addEvent(event);
        }
    } // namespace input
} // namespace ouzel
