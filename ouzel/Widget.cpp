// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Widget.h"

namespace ouzel
{
    namespace gui
    {
        Widget::Widget()
        {
            _receiveInput = true;
        }

        void Widget::setEnabled(bool enabled)
        {
            _enabled = enabled;
        }
    } // namespace gui
} // namespace ouzel
