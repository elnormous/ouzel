// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "EditBox.hpp"

namespace ouzel
{
    namespace gui
    {
        EditBox::EditBox()
        {
            pickable = true;
        }

        void EditBox::setValue(const std::string& newValue)
        {
            value = newValue;
        }

    } // namespace gui
} // namespace ouzel
