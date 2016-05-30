// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Widget.h"

namespace ouzel
{
    namespace gui
    {
        Widget::Widget()
        {
            receiveInput = true;
        }

        void Widget::setEnabled(bool newEnabled)
        {
            enabled = newEnabled;
        }
    } // namespace gui
} // namespace ouzel
