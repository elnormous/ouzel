// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef MENU_HPP
#define MENU_HPP

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

            virtual void addWidget(Widget* widget);

            template<typename T> void addWidget(const std::unique_ptr<T>& widget)
            {
                addWidget(widget.get());
            }

            template<typename T> void addWidget(std::unique_ptr<T>&& widget)
            {
                addWidget(widget.get());
                ownedChildren.push_back(std::move(widget));
            }

            void selectWidget(Widget* widget);
            virtual void selectNextWidget();
            virtual void selectPreviousWidget();

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

#endif // MENU_HPP
