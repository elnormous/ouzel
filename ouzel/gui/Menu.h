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

            virtual void addWidget(Widget* widget);
            virtual bool removeChild(Node* node) override;

            virtual void selectNextWidget();
            virtual void selectPreviousWidget();

        protected:
            virtual void enter() override;
            virtual void leave() override;

            void selectWidget(Widget* widget);

            bool handleKeyboard(Event::Type type, const KeyboardEvent& event);
            bool handleGamepad(Event::Type type, const GamepadEvent& event);
            bool handleUI(Event::Type type, const UIEvent& event);

            std::vector<WidgetPtr> widgets;
            WidgetPtr selectedWidget;

            EventHandler eventHandler;
        };
    } // namespace gui
} // namespace ouzel
