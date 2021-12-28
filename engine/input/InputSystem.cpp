// Ouzel by Elviss Strazdins

#include "InputSystem.hpp"
#include "InputManager.hpp"
#include "../core/Engine.hpp"

namespace ouzel::input
{
    void InputSystem::addCommand(const Command& command)
    {
        engine->executeOnMainThread(std::bind(&InputSystem::executeCommand, this, command));
    }

    std::future<bool> InputSystem::sendEvent(const Event& event)
    {
        std::pair<std::promise<bool>, InputSystem::Event> p{std::promise<bool>(), event};
        auto f = p.first.get_future();

        std::scoped_lock lock{eventQueueMutex};
        eventQueue.push(std::move(p));

        return f;
    }

    std::queue<std::pair<std::promise<bool>, InputSystem::Event>> InputSystem::getEvents()
    {
        std::scoped_lock lock{eventQueueMutex};
        auto result = std::move(eventQueue);
        eventQueue = {};
        return result;
    }

    void InputSystem::addInputDevice(InputDevice& inputDevice)
    {
        inputDevices.insert(std::pair(inputDevice.getId(), &inputDevice));
    }

    void InputSystem::removeInputDevice(const InputDevice& inputDevice)
    {
        if (const auto i = inputDevices.find(inputDevice.getId()); i != inputDevices.end())
            inputDevices.erase(i);
    }

    InputDevice* InputSystem::getInputDevice(DeviceId id) noexcept
    {
        if (const auto i = inputDevices.find(id); i != inputDevices.end())
            return i->second;
        else
            return nullptr;
    }
}
