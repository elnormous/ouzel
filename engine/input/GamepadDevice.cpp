// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "GamepadDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        GamepadDevice::GamepadDevice(InputSystem& initInputSystem, DeviceId initId):
            InputDevice(initInputSystem, initId, Controller::Type::gamepad)
        {
            InputSystem::Event deviceConnectEvent;
            deviceConnectEvent.type = InputSystem::Event::Type::deviceConnect;
            deviceConnectEvent.deviceId = id;
            deviceConnectEvent.deviceType = type;
            inputSystem.sendEvent(deviceConnectEvent);
        }

        GamepadDevice::~GamepadDevice()
        {
            InputSystem::Event deviceDisconnectEvent;
            deviceDisconnectEvent.type = InputSystem::Event::Type::deviceDisconnect;
            deviceDisconnectEvent.deviceId = id;
            deviceDisconnectEvent.deviceType = type;
            inputSystem.sendEvent(deviceDisconnectEvent);
        }

        std::future<bool> GamepadDevice::handleButtonValueChange(Gamepad::Button button, bool pressed, float value)
        {
            InputSystem::Event event(InputSystem::Event::Type::gamepadButtonChange);
            event.deviceId = id;
            event.gamepadButton = button;
            event.pressed = pressed;
            event.value = value;

            return inputSystem.sendEvent(event);
        }
    } // namespace input
} // namespace ouzel
