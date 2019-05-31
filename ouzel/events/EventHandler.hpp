// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_EVENTS_EVENTHANDLER_HPP
#define OUZEL_EVENTS_EVENTHANDLER_HPP

#include <functional>
#include <cstdint>
#include "events/Event.hpp"
#include "events/EventDispatcher.hpp"

namespace ouzel
{
    class EventHandler final
    {
        friend EventDispatcher;
    public:
        static constexpr int32_t PRIORITY_MAX = 0x1000;

        explicit EventHandler(int32_t initPriority = 0): priority(initPriority) {}
        ~EventHandler()
        {
            if (eventDispatcher) eventDispatcher->removeEventHandler(*this);
        }

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
        int32_t priority;
        EventDispatcher* eventDispatcher = nullptr;
    };
}

#endif // OUZEL_EVENTS_EVENTHANDLER_HPP
