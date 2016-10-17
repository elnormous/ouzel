// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include <cstdint>
#include "events/Event.h"

namespace ouzel
{
    class EventDispatcher;

    class EventHandler
    {
        friend EventDispatcher;
    public:
        static const int32_t PRIORITY_MAX = 0x1000;

        EventHandler(int32_t aPriority = 0): priority(aPriority) { }
        ~EventHandler();

        std::function<bool(Event::Type, const KeyboardEvent&)> keyboardHandler;
        std::function<bool(Event::Type, const MouseEvent&)> mouseHandler;
        std::function<bool(Event::Type, const TouchEvent&)> touchHandler;
        std::function<bool(Event::Type, const GamepadEvent&)> gamepadHandler;
        std::function<bool(Event::Type, const WindowEvent&)> windowHandler;
        std::function<bool(Event::Type, const SystemEvent&)> systemHandler;
        std::function<bool(Event::Type, const UIEvent&)> uiHandler;
        std::function<bool(Event::Type, const UserEvent&)> userHandler;

    protected:
        int32_t priority;
    };
}
