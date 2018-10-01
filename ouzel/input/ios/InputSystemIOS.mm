// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <unordered_map>
#import <UIKit/UIPress.h>
#include "InputSystemIOS.hpp"
#include "GamepadDeviceIOS.hpp"

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

        Keyboard::Key InputSystemIOS::convertKeyCode(NSInteger keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
        }

        InputSystemIOS::InputSystemIOS()
        {
            Event keyboardConnectEvent;
            keyboardConnectEvent.type = Event::Type::DEVICE_CONNECT;
            std::unique_ptr<KeyboardDevice> keyboardDeviceIOS(new KeyboardDevice(*this, ++lastDeviceId));
            keyboardConnectEvent.deviceId = keyboardDeviceIOS->getId();
            keyboardConnectEvent.deviceType = Controller::Type::KEYBOARD;
            keyboardDevice = keyboardDeviceIOS.get();
            inputDevices.insert(std::make_pair(keyboardDeviceIOS->getId(), std::move(keyboardDeviceIOS)));
            addEvent(keyboardConnectEvent);

            Event touchpadConnectEvent;
            touchpadConnectEvent.type = Event::Type::DEVICE_CONNECT;
            std::unique_ptr<TouchpadDevice> touchpadDeviceIOS(new TouchpadDevice(*this, ++lastDeviceId));
            touchpadConnectEvent.deviceId = touchpadDeviceIOS->getId();
            touchpadConnectEvent.deviceType = Controller::Type::TOUCHPAD;
            touchpadDevice = touchpadDeviceIOS.get();
            inputDevices.insert(std::make_pair(touchpadDeviceIOS->getId(), std::move(touchpadDeviceIOS)));
            addEvent(touchpadConnectEvent);
        }
    } // namespace input
} // namespace ouzel
