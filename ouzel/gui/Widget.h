// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Node.h"

namespace ouzel
{
    namespace gui
    {
        class Widget: public scene::Node
        {
        public:
            Widget();

            virtual void setEnabled(bool newEnabled);
            bool isEnabled() const { return enabled; }

        protected:
            bool enabled = true;
        };
    } // namespace gui
} // namespace ouzel
