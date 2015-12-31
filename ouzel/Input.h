// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Noncopyable.h"
#include "ReferenceCounted.h"

namespace ouzel
{
    class Engine;
    
    class Input: public Noncopyable, public ReferenceCounted
    {
        friend Engine;
    public:
        static Input* getInstance();
        
        virtual ~Input();
        
    protected:
        Input();
    };
}
