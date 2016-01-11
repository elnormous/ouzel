// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Node.h"

namespace ouzel
{
    class Widget: public Node
    {
    public:
        virtual void setEnabled(bool enabled);
        bool isEnabled() const { return _enabled; }
        
    protected:
        bool _enabled = true;
    };
}
