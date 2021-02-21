// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "TouchpadDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel::input
{
    TouchpadDevice::TouchpadDevice(InputSystem& initInputSystem, DeviceId initId, bool screen):
        InputDevice(initInputSystem, initId, Controller::Type::touchpad)
    {
        InputSystem::Event deviceConnectEvent(InputSystem::Event::Type::deviceConnect);
        deviceConnectEvent.deviceId = id;
        deviceConnectEvent.deviceType = type;
        deviceConnectEvent.screen = screen;
        inputSystem.sendEvent(deviceConnectEvent);
    }

    TouchpadDevice::~TouchpadDevice()
    {
        InputSystem::Event deviceDisconnectEvent(InputSystem::Event::Type::deviceDisconnect);
        deviceDisconnectEvent.deviceId = id;
        deviceDisconnectEvent.deviceType = type;
        inputSystem.sendEvent(deviceDisconnectEvent);
    }

    std::future<bool> TouchpadDevice::handleTouchBegin(std::uint64_t touchId, const Vector2F& position, float force)
    {
        InputSystem::Event event(InputSystem::Event::Type::touchBegin);
        event.deviceId = id;
        event.touchId = touchId;
        event.position = position;
        event.force = force;
        return inputSystem.sendEvent(event);
    }

    std::future<bool> TouchpadDevice::handleTouchEnd(std::uint64_t touchId, const Vector2F& position, float force)
    {
        InputSystem::Event event(InputSystem::Event::Type::touchEnd);
        event.deviceId = id;
        event.touchId = touchId;
        event.position = position;
        event.force = force;
        return inputSystem.sendEvent(event);
    }

    std::future<bool> TouchpadDevice::handleTouchMove(std::uint64_t touchId, const Vector2F& position, float force)
    {
        InputSystem::Event event(InputSystem::Event::Type::touchMove);
        event.deviceId = id;
        event.touchId = touchId;
        event.position = position;
        event.force = force;
        return inputSystem.sendEvent(event);
    }

    std::future<bool> TouchpadDevice::handleTouchCancel(std::uint64_t touchId, const Vector2F& position, float force)
    {
        InputSystem::Event event(InputSystem::Event::Type::touchCancel);
        event.deviceId = id;
        event.touchId = touchId;
        event.position = position;
        event.force = force;
        return inputSystem.sendEvent(event);
    }
}
