// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GUI_MENU_HPP
#define OUZEL_GUI_MENU_HPP

#include "gui/Widget.hpp"
#include "events/EventHandler.hpp"

namespace ouzel
{
    namespace gui
    {
        class Menu: public Widget
        {
        public:
            Menu();

            void setEnabled(bool newEnabled) override;

            bool removeChild(Actor* actor) override;

            void addWidget(Widget* widget);

            void selectWidget(Widget* widget);
            void selectNextWidget();
            void selectPreviousWidget();

        protected:
            void enter() override;
            void leave() override;

            bool handleKeyboard(const KeyboardEvent& event);
            bool handleGamepad(const GamepadEvent& event);
            bool handleUI(const UIEvent& event);

            std::vector<Widget*> widgets;
            Widget* selectedWidget = nullptr;

            EventHandler eventHandler;
        };
    } // namespace gui
} // namespace ouzel

#endif // OUZEL_GUI_MENU_HPP
