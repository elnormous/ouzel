// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Noncopyable.h"

namespace ouzel
{
    class Engine;
    
    class Input: public Noncopyable
    {
        friend Engine;
    public:
        static Input* getInstance();
        
        virtual ~Input();
        
        virtual void startDiscovery();
        virtual void stopDiscovery();
        
    protected:
        Input();
    };
}
