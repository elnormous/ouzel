// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Widget.h"
#include "Menu.h"

namespace ouzel
{
    namespace gui
    {
        Widget::Widget()
        {
        }

        Widget::~Widget()
        {
            if (menu) menu->removeWidget(this);
        }

        void Widget::setMenu(Menu* newMenu)
        {
            if (menu)
            {
                menu->removeWidget(this);
            }

            menu = newMenu;

            if (menu)
            {
                menu->addWidget(this);
            }
        }

        void Widget::removeFromMenu()
        {
            if (menu)
            {
                menu->removeWidget(this);
            }
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
