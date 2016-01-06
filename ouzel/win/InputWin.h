// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Input.h"

namespace ouzel
{
    class Engine;
    
    class InputWin: public Input
    {
        friend Engine;
    public:
        
        virtual ~InputWin();
        
        virtual void startDiscovery() override;
        virtual void stopDiscovery() override;
        
    protected:
        InputWin();
    };
}
