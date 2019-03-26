// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GUI_WIDGET_HPP
#define OUZEL_GUI_WIDGET_HPP

#include "scene/Actor.hpp"

namespace ouzel
{
    namespace gui
    {
        class Menu;

        class Widget: public scene::Actor
        {
            friend Menu;
        public:
            Widget();

            inline Menu* getMenu() const { return menu; }

            virtual void setEnabled(bool newEnabled);
            inline bool isEnabled() const { return enabled; }

            inline bool isSelected() const { return selected; }

        protected:
            virtual void setSelected(bool newSelected);

            Menu* menu = nullptr;
            bool enabled = true;
            bool selected = false;
        };
    } // namespace gui
} // namespace ouzel

#endif // OUZEL_GUI_WIDGET_HPP
