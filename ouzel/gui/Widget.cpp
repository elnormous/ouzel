// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Widget.h"

namespace ouzel
{
    namespace gui
    {
        Widget::Widget()
        {
        }

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
