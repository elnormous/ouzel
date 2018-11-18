// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "InputSystem.hpp"
#include "InputManager.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace input
    {
        InputSystem::InputSystem(const std::function<std::future<bool>(const Event&)>& initCallback):
            callback(initCallback)
        {
        }

        void InputSystem::addCommand(const Command& command)
        {
            engine->executeOnMainThread(std::bind(&InputSystem::executeCommand, this, command));
        }

        std::future<bool> InputSystem::sendEvent(const Event& event)
        {
            return callback(event);
        }

        void InputSystem::addInputDevice(InputDevice& inputDevice)
        {
            Event deviceConnectEvent;
            deviceConnectEvent.type = Event::Type::DEVICE_CONNECT;
            deviceConnectEvent.deviceId = inputDevice.getId();
            deviceConnectEvent.deviceType = inputDevice.getType();
            inputDevices.insert(std::make_pair(inputDevice.getId(), &inputDevice));
            sendEvent(deviceConnectEvent);
        }

        void InputSystem::removeInputDevice(const InputDevice& inputDevice)
        {
            auto i = inputDevices.find(inputDevice.getId());

            if (i != inputDevices.end())
            {
                Event deviceDisconnectEvent;
                deviceDisconnectEvent.type = Event::Type::DEVICE_DISCONNECT;
                deviceDisconnectEvent.deviceId = inputDevice.getId();
                deviceDisconnectEvent.deviceType = inputDevice.getType();
                sendEvent(deviceDisconnectEvent);
                inputDevices.erase(i);
            }
        }

        InputDevice* InputSystem::getInputDevice(uint32_t id)
        {
            auto i = inputDevices.find(id);

            if (i != inputDevices.end())
                return i->second;
            else
                return nullptr;
        }
    } // namespace input
} // namespace ouzel
