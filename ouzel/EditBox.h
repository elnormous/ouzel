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

            void setValue(const std::string& newValue);
            const std::string& getValue() const { return value; }

        protected:
            std::string value;
        };
    } // namespace gui
} // namespace ouzel
