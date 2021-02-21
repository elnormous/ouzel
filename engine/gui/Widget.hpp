// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GUI_WIDGET_HPP
#define OUZEL_GUI_WIDGET_HPP

#include "../scene/Actor.hpp"

namespace ouzel::gui
{
    class Menu;

    class Widget: public scene::Actor
    {
        friend Menu;
    public:
        Widget() = default;

        auto getMenu() const noexcept { return menu; }

        virtual void setEnabled(bool newEnabled);
        auto isEnabled() const noexcept { return enabled; }

        auto isSelected() const noexcept { return selected; }

    protected:
        virtual void setSelected(bool newSelected);

        Menu* menu = nullptr;
        bool enabled = true;
        bool selected = false;
    };
}

#endif // OUZEL_GUI_WIDGET_HPP
