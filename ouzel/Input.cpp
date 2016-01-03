// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Input.h"

namespace ouzel
{
    static Input* sharedInput = nullptr;
    
    Input* Input::getInstance()
    {
        return sharedInput;
    }
    
    Input::Input()
    {
        
    }
    
    Input::~Input()
    {
        
    }
    
    void Input::startDiscovery()
    {
        
    }
    
    void Input::stopDiscovery()
    {
        
    }
}
