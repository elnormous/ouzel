// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Types.h"
#include "gui/Widget.h"
#include "events/EventHandler.h"

namespace ouzel
{
    namespace gui
    {
        class Menu: public Widget
        {
        public:
            Menu();
            virtual ~Menu();

            virtual bool addWidget(const WidgetPtr& widget);
            virtual bool removeWidget(const WidgetPtr& widget);

        protected:
            void selectWidget(const WidgetPtr& widget);

            bool handleKeyboard(Event::Type type, const KeyboardEvent& event);
            bool handleGamepad(Event::Type type, const GamepadEvent& event);
            bool handleUI(Event::Type type, const UIEvent& event);

            std::list<WidgetPtr> widgets;
            WidgetPtr selectedWidget;

            EventHandler eventHandler;
        };
    } // namespace gui
} // namespace ouzel
