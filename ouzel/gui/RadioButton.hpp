// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GUI_RADIOBUTTON_HPP
#define OUZEL_GUI_RADIOBUTTON_HPP

#include "gui/Widget.hpp"

namespace ouzel
{
    namespace gui
    {
        class RadioButton: public Widget
        {
        public:
            RadioButton();

            void setEnabled(bool newEnabled) override;

            void setChecked(bool newChecked);
            inline bool isChecked() const { return checked; }

        private:
            bool pointerOver = false;
            bool pressed = false;
            bool checked = false;
        };
    } // namespace gui
} // namespace ouzel

#endif // OUZEL_GUI_RADIOBUTTON_HPP
