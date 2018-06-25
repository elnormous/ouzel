// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <string>
#include "gui/Widget.hpp"

namespace ouzel
{
    namespace gui
    {
        class EditBox: public Widget
        {
        public:
            EditBox();

            void setValue(const std::string& newValue);
            inline const std::string& getValue() const { return value; }

        protected:
            std::string value;
        };
    } // namespace gui
} // namespace ouzel
