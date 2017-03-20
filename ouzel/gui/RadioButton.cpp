// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RadioButton.h"

namespace ouzel
{
    namespace gui
    {
        RadioButton::RadioButton()
        {
            pickable = true;
        }

        void RadioButton::setEnabled(bool enabled)
        {
            Widget::setEnabled(enabled);

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
