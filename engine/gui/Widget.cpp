// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "Widget.hpp"

namespace ouzel::gui
{
    void Widget::setEnabled(bool newEnabled)
    {
        enabled = newEnabled;
    }

    void Widget::setSelected(bool newSelected)
    {
        selected = newSelected;
    }
}
