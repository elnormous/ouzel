// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GUI_EDITBOX_HPP
#define OUZEL_GUI_EDITBOX_HPP

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

            inline const std::string& getValue() const { return value; }
            void setValue(const std::string& newValue);

        protected:
            std::string value;
        };
    } // namespace gui
} // namespace ouzel

#endif // OUZEL_GUI_EDITBOX_HPP
