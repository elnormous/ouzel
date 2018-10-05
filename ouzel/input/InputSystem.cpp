// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "InputSystem.hpp"
#include "InputManager.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace input
    {
        InputSystem::InputSystem()
        {
        }

        void InputSystem::addCommand(const Command& command)
        {
            engine->executeOnMainThread(std::bind(&InputSystem::executeCommand, this, command));
        }

        std::vector<InputSystem::Event> InputSystem::getEvents() const
        {
            std::vector<Event> result;
            std::unique_lock<std::mutex> lock(eventQueueMutex);
            while (!eventQueue.empty())
            {
                result.push_back(eventQueue.front());
                eventQueue.pop();
            }
            return result;
        }

        void InputSystem::addEvent(const Event& event)
        {
            std::unique_lock<std::mutex> lock(eventQueueMutex);
            eventQueue.push(event);
        }

        void InputSystem::addInputDevice(std::unique_ptr<InputDevice>&& inputDevice)
        {
            Event deviceConnectEvent;
            deviceConnectEvent.type = Event::Type::DEVICE_CONNECT;
            deviceConnectEvent.deviceId = inputDevice->getId();
            deviceConnectEvent.deviceType = inputDevice->getType();
            inputDevices.insert(std::make_pair(inputDevice->getId(), std::move(inputDevice)));
            addEvent(deviceConnectEvent);
        }

        void InputSystem::removeInputDevice(const InputDevice* inputDevice)
        {
            auto i = inputDevices.find(inputDevice->getId());

            if (i != inputDevices.end())
            {
                Event deviceDisconnectEvent;
                deviceDisconnectEvent.type = Event::Type::DEVICE_DISCONNECT;
                deviceDisconnectEvent.deviceId = inputDevice->getId();
                deviceDisconnectEvent.deviceType = inputDevice->getType();
                addEvent(deviceDisconnectEvent);
                inputDevices.erase(i);
            }
        }
    } // namespace input
} // namespace ouzel
