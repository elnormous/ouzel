// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "Widget.hpp"

namespace ouzel
{
    namespace gui
    {
        void Widget::setEnabled(bool newEnabled)
        {
            enabled = newEnabled;
        }

        void Widget::setSelected(bool newSelected)
        {
            selected = newSelected;
        }
    } // namespace gui
} // namespace ouzel
