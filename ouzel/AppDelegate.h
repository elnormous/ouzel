// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Engine;
    
    class AppDelegate
    {
    public:
        virtual void begin(Engine* engine) {}
    };
}