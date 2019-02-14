// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
