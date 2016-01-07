// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Settings.h"

namespace ouzel
{
    class App
    {
    public:
        virtual ~App() {}
        virtual Settings getSettings() = 0;
        
        virtual void begin() = 0;
    };
}
