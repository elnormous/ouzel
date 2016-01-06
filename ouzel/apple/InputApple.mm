// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <GameController/GameController.h>
#include "InputApple.h"

@interface ConnectDelegate: NSObject
{
    
}

-(void)handleControllerConnected:(NSNotification*)notification;
-(void)handleControllerDisconnected:(NSNotification*)notification;

@end

@implementation ConnectDelegate

-(void)handleControllerConnected:(NSNotification*)notification
{
    
}

-(void)handleControllerDisconnected:(NSNotification*)notification
{
    
}

@end

namespace ouzel
{
    InputApple::InputApple()
    {
        
    }
    
    InputApple::~InputApple()
    {
        
    }
    
    void InputApple::startDiscovery()
    {
        
    }
    
    void InputApple::stopDiscovery()
    {
        
    }
}
