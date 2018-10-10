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

        std::vector<std::pair<std::promise<bool>, InputSystem::Event>> InputSystem::getEvents() const
        {
            std::vector<std::pair<std::promise<bool>, InputSystem::Event>> result;
            std::unique_lock<std::mutex> lock(eventQueueMutex);
            while (!eventQueue.empty())
            {
                result.push_back(std::move(eventQueue.front()));
                eventQueue.pop();
            }
            return result;
        }

        std::future<bool> InputSystem::postEvent(const Event& event)
        {
            std::pair<std::promise<bool>, Event> p(std::promise<bool>(), event);
            std::future<bool> f = p.first.get_future();

            std::unique_lock<std::mutex> lock(eventQueueMutex);
            eventQueue.push(std::move(p));

            return f;
        }

        void InputSystem::addInputDevice(InputDevice& inputDevice)
        {
            Event deviceConnectEvent;
            deviceConnectEvent.type = Event::Type::DEVICE_CONNECT;
            deviceConnectEvent.deviceId = inputDevice.getId();
            deviceConnectEvent.deviceType = inputDevice.getType();
            inputDevices.insert(std::make_pair(inputDevice.getId(), &inputDevice));
            postEvent(deviceConnectEvent);
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
                postEvent(deviceDisconnectEvent);
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
