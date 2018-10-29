// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

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
            bool isChecked() const { return checked; }

        protected:
            bool pointerOver = false;
            bool pressed = false;
            bool checked = false;
        };
    } // namespace gui
} // namespace ouzel
