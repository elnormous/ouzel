// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

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
            bool isEnabled() const { return enabled; }

            bool isSelected() const { return selected; }

        protected:
            virtual void setSelected(bool newSelected);

            Menu* menu = nullptr;
            bool enabled = true;
            bool selected = false;
        };
    } // namespace gui
} // namespace ouzel
