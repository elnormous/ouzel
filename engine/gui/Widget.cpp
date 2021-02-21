// Ouzel by Elviss Strazdins

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
