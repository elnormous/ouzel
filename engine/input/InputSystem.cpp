// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "InputSystem.hpp"
#include "InputManager.hpp"
#include "../core/Engine.hpp"

namespace ouzel::input
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
        inputDevices.insert(std::pair(inputDevice.getId(), &inputDevice));
    }

    void InputSystem::removeInputDevice(const InputDevice& inputDevice)
    {
        const auto i = inputDevices.find(inputDevice.getId());

        if (i != inputDevices.end())
            inputDevices.erase(i);
    }

    InputDevice* InputSystem::getInputDevice(DeviceId id)
    {
        const auto i = inputDevices.find(id);

        if (i != inputDevices.end())
            return i->second;
        else
            return nullptr;
    }
}
