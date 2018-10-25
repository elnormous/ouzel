// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

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

            virtual void setEnabled(bool newEnabled) override;

            virtual bool removeChild(Actor* actor) override;

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
            virtual void enter() override;
            virtual void leave() override;

            bool handleKeyboard(const KeyboardEvent& event);
            bool handleGamepad(const GamepadEvent& event);
            bool handleUI(const UIEvent& event);

            std::vector<Widget*> widgets;
            Widget* selectedWidget = nullptr;

            EventHandler eventHandler;
        };
    } // namespace gui
} // namespace ouzel
