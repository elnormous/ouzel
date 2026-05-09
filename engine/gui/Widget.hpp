#ifndef FLUXION_GUI_WIDGET_HPP
#define FLUXION_GUI_WIDGET_HPP

#include "../scene/Actor.hpp"

namespace fluxion::gui
{
    class Menu;

    class Widget: public scene::Actor
    {
        friend Menu;
    public:
        Widget() = default;

        auto getMenu() const noexcept { return menu; }

        virtual void setEnabled(bool newEnabled) { enabled = newEnabled; }
        auto isEnabled() const noexcept { return enabled; }

        auto isSelected() const noexcept { return selected; }

    protected:
        virtual void setSelected(bool newSelected) { selected = newSelected; }

        Menu* menu = nullptr;
        bool enabled = true;
        bool selected = false;
    };
}

#endif // FLUXION_GUI_WIDGET_HPP
