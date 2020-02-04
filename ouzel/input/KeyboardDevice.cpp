// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "KeyboardDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        KeyboardDevice::KeyboardDevice(InputSystem& initInputSystem, uintptr_t initId):
            InputDevice(initInputSystem, initId, Controller::Type::Keyboard)
        {
            InputSystem::Event deviceConnectEvent;
            deviceConnectEvent.type = InputSystem::Event::Type::DeviceConnect;
            deviceConnectEvent.deviceId = id;
            deviceConnectEvent.deviceType = type;
            inputSystem.sendEvent(deviceConnectEvent);
        }

        KeyboardDevice::~KeyboardDevice()
        {
            InputSystem::Event deviceDisconnectEvent;
            deviceDisconnectEvent.type = InputSystem::Event::Type::DeviceDisconnect;
            deviceDisconnectEvent.deviceId = id;
            deviceDisconnectEvent.deviceType = type;
            inputSystem.sendEvent(deviceDisconnectEvent);
        }

        std::future<bool> KeyboardDevice::handleKeyPress(Keyboard::Key key)
        {
            InputSystem::Event event(InputSystem::Event::Type::KeyboardKeyPress);
            event.deviceId = id;
            event.keyboardKey = key;
            return inputSystem.sendEvent(event);
        }

        std::future<bool> KeyboardDevice::handleKeyRelease(Keyboard::Key key)
        {
            InputSystem::Event event(InputSystem::Event::Type::KeyboardKeyRelease);
            event.deviceId = id;
            event.keyboardKey = key;
            return inputSystem.sendEvent(event);
        }
    } // namespace input
} // namespace ouzel
