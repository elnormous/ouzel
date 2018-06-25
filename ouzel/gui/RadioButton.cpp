// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "RadioButton.hpp"

namespace ouzel
{
    namespace gui
    {
        RadioButton::RadioButton()
        {
            pickable = true;
        }

        void RadioButton::setEnabled(bool newEnabled)
        {
            Widget::setEnabled(newEnabled);

            selected = false;
            pointerOver = false;
            pressed = false;
        }

        void RadioButton::setChecked(bool newChecked)
        {
            checked = newChecked;
        }
    } // namespace gui
} // namespace ouzel
