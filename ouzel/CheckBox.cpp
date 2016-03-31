// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CheckBox.h"

namespace ouzel
{
    namespace gui
    {
        CheckBox::CheckBox()
        {

        }

        CheckBox::~CheckBox()
        {

        }

        bool CheckBox::init(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled, const std::string& tick)
        {
            return true;
        }

        void CheckBox::setEnabled(bool enabled)
        {
            Widget::setEnabled(enabled);
        }
        
    } // namespace gui
} // namespace ouzel
