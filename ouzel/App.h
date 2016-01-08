// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Settings.h"

namespace ouzel
{
    class App
    {
    public:
        virtual ~App() = 0;
        virtual Settings getSettings() = 0;
        
        virtual void begin() = 0;
    };
    
    inline App::~App()
    {
    }
}
