// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include <cstdint>
#include "events/Event.hpp"

namespace ouzel
{
    class EventDispatcher;

    class EventHandler
    {
        friend EventDispatcher;
    public:
        static const int32_t PRIORITY_MAX = 0x1000;

        explicit EventHandler(int32_t initPriority = 0): priority(initPriority) {}
        ~EventHandler();

        void remove();

        std::function<bool(Event::Type, const KeyboardEvent&)> keyboardHandler;
        std::function<bool(Event::Type, const MouseEvent&)> mouseHandler;
        std::function<bool(Event::Type, const TouchEvent&)> touchHandler;
        std::function<bool(Event::Type, const GamepadEvent&)> gamepadHandler;
        std::function<bool(Event::Type, const WindowEvent&)> windowHandler;
        std::function<bool(Event::Type, const SystemEvent&)> systemHandler;
        std::function<bool(Event::Type, const UIEvent&)> uiHandler;
        std::function<bool(Event::Type, const AnimationEvent&)> animationHandler;
        std::function<bool(Event::Type, const SoundEvent&)> soundHandler;
        std::function<bool(Event::Type, const UserEvent&)> userHandler;

    private:
        int32_t priority;
        EventDispatcher* eventDispatcher = nullptr;
    };
}
