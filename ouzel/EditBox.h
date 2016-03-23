// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Widget.h"

namespace ouzel
{
    namespace gui
    {
        class EditBox: public Widget
        {
        public:
            EditBox();
            virtual ~EditBox();

            void setValue(const std::string& value);
            const std::string& getValue() const { return _value; }

        private:
            std::string _value;
        };
    } // namespace gui
} // namespace ouzel
