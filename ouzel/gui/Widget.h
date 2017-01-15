// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Node.h"

namespace ouzel
{
    namespace gui
    {
        class Menu;

        class Widget: public scene::Node
        {
            friend Menu;
        public:
            Widget();

            virtual void setEnabled(bool newEnabled);
            bool isEnabled() const { return enabled; }

            bool isSelected() const { return selected; }

        protected:
            virtual void setSelected(bool newSelected);

            bool enabled = true;
            bool selected = false;
        };
    } // namespace gui
} // namespace ouzel
