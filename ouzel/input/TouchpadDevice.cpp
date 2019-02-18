// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "TouchpadDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        TouchpadDevice::TouchpadDevice(InputSystem& initInputSystem, uint32_t initId, bool screen):
            InputDevice(initInputSystem, initId, Controller::Type::TOUCHPAD)
        {
            InputSystem::Event deviceConnectEvent;
            deviceConnectEvent.type = InputSystem::Event::Type::DEVICE_CONNECT;
            deviceConnectEvent.deviceId = id;
            deviceConnectEvent.deviceType = type;
            deviceConnectEvent.screen = screen;
            inputSystem.sendEvent(deviceConnectEvent);
        }

        TouchpadDevice::~TouchpadDevice()
        {
            InputSystem::Event deviceDisconnectEvent;
            deviceDisconnectEvent.type = InputSystem::Event::Type::DEVICE_DISCONNECT;
            deviceDisconnectEvent.deviceId = id;
            deviceDisconnectEvent.deviceType = type;
            inputSystem.sendEvent(deviceDisconnectEvent);
        }

        std::future<bool> TouchpadDevice::handleTouchBegin(uint64_t touchId, const Vector2F& position, float force)
        {
            InputSystem::Event event(InputSystem::Event::Type::TOUCH_BEGIN);
            event.deviceId = id;
            event.touchId = touchId;
            event.position = position;
            event.force = force;
            return inputSystem.sendEvent(event);
        }

        std::future<bool> TouchpadDevice::handleTouchEnd(uint64_t touchId, const Vector2F& position, float force)
        {
            InputSystem::Event event(InputSystem::Event::Type::TOUCH_END);
            event.deviceId = id;
            event.touchId = touchId;
            event.position = position;
            event.force = force;
            return inputSystem.sendEvent(event);
        }

        std::future<bool> TouchpadDevice::handleTouchMove(uint64_t touchId, const Vector2F& position, float force)
        {
            InputSystem::Event event(InputSystem::Event::Type::TOUCH_MOVE);
            event.deviceId = id;
            event.touchId = touchId;
            event.position = position;
            event.force = force;
            return inputSystem.sendEvent(event);
        }

        std::future<bool> TouchpadDevice::handleTouchCancel(uint64_t touchId, const Vector2F& position, float force)
        {
            InputSystem::Event event(InputSystem::Event::Type::TOUCH_CANCEL);
            event.deviceId = id;
            event.touchId = touchId;
            event.position = position;
            event.force = force;
            return inputSystem.sendEvent(event);
        }
    } // namespace input
} // namespace ouzel
