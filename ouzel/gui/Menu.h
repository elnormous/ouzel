// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

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

            virtual void setEnabled(bool newEnabled) override;

            virtual void addWidget(const std::shared_ptr<Widget>& widget);
            virtual bool removeChild(Node* node) override;

            void selectWidget(Widget* widget);
            virtual void selectNextWidget();
            virtual void selectPreviousWidget();

        protected:
            virtual void enter() override;
            virtual void leave() override;

            bool handleKeyboard(Event::Type type, const KeyboardEvent& event);
            bool handleGamepad(Event::Type type, const GamepadEvent& event);
            bool handleUI(Event::Type type, const UIEvent& event);

            std::vector<std::shared_ptr<Widget>> widgets;
            Widget* selectedWidget = nullptr;

            EventHandler eventHandler;
        };
    } // namespace gui
} // namespace ouzel
