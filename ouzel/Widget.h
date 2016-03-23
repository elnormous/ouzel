// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Node.h"

namespace ouzel
{
    namespace gui
    {
        class Widget: public scene::Node
        {
        public:
            virtual void setEnabled(bool enabled);
            bool isEnabled() const { return _enabled; }

        protected:
            bool _enabled = true;
        };
    } // namespace gui
} // namespace ouzel
