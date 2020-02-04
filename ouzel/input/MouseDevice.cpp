// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "MouseDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        MouseDevice::MouseDevice(InputSystem& initInputSystem, uintptr_t initId):
            InputDevice(initInputSystem, initId, Controller::Type::Mouse)
        {
            InputSystem::Event deviceConnectEvent;
            deviceConnectEvent.type = InputSystem::Event::Type::DeviceConnect;
            deviceConnectEvent.deviceId = id;
            deviceConnectEvent.deviceType = type;
            inputSystem.sendEvent(deviceConnectEvent);
        }

        MouseDevice::~MouseDevice()
        {
            InputSystem::Event deviceDisconnectEvent;
            deviceDisconnectEvent.type = InputSystem::Event::Type::DeviceDisconnect;
            deviceDisconnectEvent.deviceId = id;
            deviceDisconnectEvent.deviceType = type;
            inputSystem.sendEvent(deviceDisconnectEvent);
        }

        std::future<bool> MouseDevice::handleButtonPress(Mouse::Button button, const Vector2F& position)
        {
            InputSystem::Event event(InputSystem::Event::Type::MousePress);
            event.deviceId = id;
            event.mouseButton = button;
            event.position = position;
            return inputSystem.sendEvent(event);
        }

        std::future<bool> MouseDevice::handleButtonRelease(Mouse::Button button, const Vector2F& position)
        {
            InputSystem::Event event(InputSystem::Event::Type::MouseRelease);
            event.deviceId = id;
            event.mouseButton = button;
            event.position = position;
            return inputSystem.sendEvent(event);
        }

        std::future<bool> MouseDevice::handleMove(const Vector2F& position)
        {
            InputSystem::Event event(InputSystem::Event::Type::MouseMove);
            event.deviceId = id;
            event.position = position;
            return inputSystem.sendEvent(event);
        }

        std::future<bool> MouseDevice::handleRelativeMove(const Vector2F& position)
        {
            InputSystem::Event event(InputSystem::Event::Type::MouseRelativeMove);
            event.deviceId = id;
            event.position = position;
            return inputSystem.sendEvent(event);
        }

        std::future<bool> MouseDevice::handleScroll(const Vector2F& scroll, const Vector2F& position)
        {
            InputSystem::Event event(InputSystem::Event::Type::MouseScroll);
            event.deviceId = id;
            event.position = position;
            event.scroll = scroll;
            return inputSystem.sendEvent(event);
        }

        std::future<bool> MouseDevice::handleCursorLockChange(bool locked)
        {
            InputSystem::Event event(InputSystem::Event::Type::MouseLockChanged);
            event.deviceId = id;
            event.locked = locked;
            return inputSystem.sendEvent(event);
        }
    } // namespace input
} // namespace ouzel
