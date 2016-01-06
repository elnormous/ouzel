// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Input.h"

namespace ouzel
{
    class Engine;
    
    class InputApple: public Input
    {
        friend Engine;
    public:
        
        virtual ~InputApple();
        
        virtual void startDiscovery() override;
        virtual void stopDiscovery() override;
        
    protected:
        InputApple();
    };
}
