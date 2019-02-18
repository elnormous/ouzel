// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "GamepadDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        GamepadDevice::GamepadDevice(InputSystem& initInputSystem, uint32_t initId):
            InputDevice(initInputSystem, initId, Controller::Type::GAMEPAD)
        {
            InputSystem::Event deviceConnectEvent;
            deviceConnectEvent.type = InputSystem::Event::Type::DEVICE_CONNECT;
            deviceConnectEvent.deviceId = id;
            deviceConnectEvent.deviceType = type;
            inputSystem.sendEvent(deviceConnectEvent);
        }

        GamepadDevice::~GamepadDevice()
        {
            InputSystem::Event deviceDisconnectEvent;
            deviceDisconnectEvent.type = InputSystem::Event::Type::DEVICE_DISCONNECT;
            deviceDisconnectEvent.deviceId = id;
            deviceDisconnectEvent.deviceType = type;
            inputSystem.sendEvent(deviceDisconnectEvent);
        }

        std::future<bool> GamepadDevice::handleButtonValueChange(Gamepad::Button button, bool pressed, float value)
        {
            InputSystem::Event event(InputSystem::Event::Type::GAMEPAD_BUTTON_CHANGE);
            event.deviceId = id;
            event.gamepadButton = button;
            event.pressed = pressed;
            event.value = value;

            return inputSystem.sendEvent(event);
        }
    } // namespace input
} // namespace ouzel
