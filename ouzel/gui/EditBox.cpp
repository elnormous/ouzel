// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
