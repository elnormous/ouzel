// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "InputSystem.hpp"
#include "InputManager.hpp"

namespace ouzel
{
    namespace input
    {
        InputSystem::InputSystem()
        {
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

        void InputSystem::pushEvent(const Event& event)
        {
            std::unique_lock<std::mutex> lock(eventQueueMutex);
            eventQueue.push(event);
        }
    } // namespace input
} // namespace ouzel
