// Ouzel by Elviss Strazdins

#include "KeyboardDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel::input
{
    KeyboardDevice::KeyboardDevice(InputSystem& initInputSystem, DeviceId initId):
        InputDevice{initInputSystem, initId, Controller::Type::keyboard}
    {
        InputSystem::Event deviceConnectEvent{InputSystem::Event::Type::deviceConnect};
        deviceConnectEvent.deviceId = id;
        deviceConnectEvent.deviceType = type;
        inputSystem.sendEvent(deviceConnectEvent);
    }

    KeyboardDevice::~KeyboardDevice()
    {
        InputSystem::Event deviceDisconnectEvent{InputSystem::Event::Type::deviceDisconnect};
        deviceDisconnectEvent.deviceId = id;
        deviceDisconnectEvent.deviceType = type;
        inputSystem.sendEvent(deviceDisconnectEvent);
    }

    std::future<bool> KeyboardDevice::handleKeyPress(Keyboard::Key key)
    {
        InputSystem::Event event{InputSystem::Event::Type::keyboardKeyPress};
        event.deviceId = id;
        event.keyboardKey = key;
        return inputSystem.sendEvent(event);
    }

    std::future<bool> KeyboardDevice::handleKeyRelease(Keyboard::Key key)
    {
        InputSystem::Event event{InputSystem::Event::Type::keyboardKeyRelease};
        event.deviceId = id;
        event.keyboardKey = key;
        return inputSystem.sendEvent(event);
    }
}
