// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

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

            virtual void setEnabled(bool newEnabled) override;

            virtual void setChecked(bool newChecked);
            virtual bool isChecked() const { return checked; }

        protected:
            bool pointerOver = false;
            bool pressed = false;
            bool checked = false;
        };
    } // namespace gui
} // namespace ouzel
