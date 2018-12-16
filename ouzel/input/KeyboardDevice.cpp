// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "KeyboardDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        KeyboardDevice::KeyboardDevice(InputSystem& initInputSystem, uint32_t initId):
            InputDevice(initInputSystem, initId, Controller::Type::KEYBOARD)
        {
            InputSystem::Event deviceConnectEvent;
            deviceConnectEvent.type = InputSystem::Event::Type::DEVICE_CONNECT;
            deviceConnectEvent.deviceId = id;
            deviceConnectEvent.deviceType = type;
            inputSystem.sendEvent(deviceConnectEvent);
        }

        KeyboardDevice::~KeyboardDevice()
        {
            InputSystem::Event deviceDisconnectEvent;
            deviceDisconnectEvent.type = InputSystem::Event::Type::DEVICE_DISCONNECT;
            deviceDisconnectEvent.deviceId = id;
            deviceDisconnectEvent.deviceType = type;
            inputSystem.sendEvent(deviceDisconnectEvent);
        }

        std::future<bool> KeyboardDevice::handleKeyPress(Keyboard::Key key)
        {
            InputSystem::Event event(InputSystem::Event::Type::KEY_PRESS);
            event.deviceId = id;
            event.keyboardKey = key;
            return inputSystem.sendEvent(event);
        }

        std::future<bool> KeyboardDevice::handleKeyRelease(Keyboard::Key key)
        {
            InputSystem::Event event(InputSystem::Event::Type::KEY_RELEASE);
            event.deviceId = id;
            event.keyboardKey = key;
            return inputSystem.sendEvent(event);
        }
    } // namespace input
} // namespace ouzel
