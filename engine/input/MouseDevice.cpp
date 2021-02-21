// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "MouseDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel::input
{
    MouseDevice::MouseDevice(InputSystem& initInputSystem, DeviceId initId):
        InputDevice(initInputSystem, initId, Controller::Type::mouse)
    {
        InputSystem::Event deviceConnectEvent(InputSystem::Event::Type::deviceConnect);
        deviceConnectEvent.deviceId = id;
        deviceConnectEvent.deviceType = type;
        inputSystem.sendEvent(deviceConnectEvent);
    }

    MouseDevice::~MouseDevice()
    {
        InputSystem::Event deviceDisconnectEvent(InputSystem::Event::Type::deviceDisconnect);
        deviceDisconnectEvent.deviceId = id;
        deviceDisconnectEvent.deviceType = type;
        inputSystem.sendEvent(deviceDisconnectEvent);
    }

    std::future<bool> MouseDevice::handleButtonPress(Mouse::Button button, const Vector2F& position)
    {
        InputSystem::Event event(InputSystem::Event::Type::mousePress);
        event.deviceId = id;
        event.mouseButton = button;
        event.position = position;
        return inputSystem.sendEvent(event);
    }

    std::future<bool> MouseDevice::handleButtonRelease(Mouse::Button button, const Vector2F& position)
    {
        InputSystem::Event event(InputSystem::Event::Type::mouseRelease);
        event.deviceId = id;
        event.mouseButton = button;
        event.position = position;
        return inputSystem.sendEvent(event);
    }

    std::future<bool> MouseDevice::handleMove(const Vector2F& position)
    {
        InputSystem::Event event(InputSystem::Event::Type::mouseMove);
        event.deviceId = id;
        event.position = position;
        return inputSystem.sendEvent(event);
    }

    std::future<bool> MouseDevice::handleRelativeMove(const Vector2F& position)
    {
        InputSystem::Event event(InputSystem::Event::Type::mouseRelativeMove);
        event.deviceId = id;
        event.position = position;
        return inputSystem.sendEvent(event);
    }

    std::future<bool> MouseDevice::handleScroll(const Vector2F& scroll, const Vector2F& position)
    {
        InputSystem::Event event(InputSystem::Event::Type::mouseScroll);
        event.deviceId = id;
        event.position = position;
        event.scroll = scroll;
        return inputSystem.sendEvent(event);
    }

    std::future<bool> MouseDevice::handleCursorLockChange(bool locked)
    {
        InputSystem::Event event(InputSystem::Event::Type::mouseLockChanged);
        event.deviceId = id;
        event.locked = locked;
        return inputSystem.sendEvent(event);
    }
}
