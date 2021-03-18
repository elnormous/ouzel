// Ouzel by Elviss Strazdins

#ifndef OUZEL_EVENTS_EVENTHANDLER_HPP
#define OUZEL_EVENTS_EVENTHANDLER_HPP

#include <functional>
#include <cstdint>
#include "Event.hpp"
#include "EventDispatcher.hpp"

namespace ouzel
{
    class EventHandler final
    {
        friend EventDispatcher;
    public:
        using Priority = std::int32_t;
        static constexpr Priority priorityMax = 0x1000;

        explicit EventHandler(Priority initPriority = 0): priority{initPriority} {}
        ~EventHandler()
        {
            if (eventDispatcher) eventDispatcher->removeEventHandler(*this);
        }

        EventHandler(const EventHandler&) = delete;
        EventHandler& operator=(const EventHandler&) = delete;
        EventHandler(EventHandler&&) = delete;
        EventHandler& operator=(EventHandler&&) = delete;

        void remove()
        {
            if (eventDispatcher)
            {
                eventDispatcher->removeEventHandler(*this);
                eventDispatcher = nullptr;
            }
        }

        std::function<bool(const KeyboardEvent&)> keyboardHandler;
        std::function<bool(const MouseEvent&)> mouseHandler;
        std::function<bool(const TouchEvent&)> touchHandler;
        std::function<bool(const GamepadEvent&)> gamepadHandler;
        std::function<bool(const WindowEvent&)> windowHandler;
        std::function<bool(const SystemEvent&)> systemHandler;
        std::function<bool(const UIEvent&)> uiHandler;
        std::function<bool(const AnimationEvent&)> animationHandler;
        std::function<bool(const SoundEvent&)> soundHandler;
        std::function<bool(const UpdateEvent&)> updateHandler;
        std::function<bool(const UserEvent&)> userHandler;

    private:
        Priority priority;
        EventDispatcher* eventDispatcher = nullptr;
    };
}

#endif // OUZEL_EVENTS_EVENTHANDLER_HPP
