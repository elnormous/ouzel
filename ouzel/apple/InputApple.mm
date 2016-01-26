// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CompileConfig.h"
#ifdef OUZEL_PLATFORM_OSX
#import <AppKit/AppKit.h>
#endif
#import <GameController/GameController.h>
#include "InputApple.h"
#include "Engine.h"
#include "GamepadApple.h"
#include "Utils.h"

@interface ConnectDelegate: NSObject
{
    
}

-(void)handleControllerConnected:(NSNotification*)notification;
-(void)handleControllerDisconnected:(NSNotification*)notification;

@end

@implementation ConnectDelegate

-(void)handleControllerConnected:(NSNotification*)notification
{
    std::shared_ptr<ouzel::InputApple> inputApple = std::static_pointer_cast<ouzel::InputApple>(ouzel::Engine::getInstance()->getInput());
    inputApple->handleGamepadConnected(notification.object);
}

-(void)handleControllerDisconnected:(NSNotification*)notification
{
    std::shared_ptr<ouzel::InputApple> inputApple = std::static_pointer_cast<ouzel::InputApple>(ouzel::Engine::getInstance()->getInput());
    inputApple->handleGamepadDisconnected(notification.object);
}

@end

namespace ouzel
{
    InputApple::InputApple()
    {
        _connectDelegate = [[ConnectDelegate alloc]init];
        
        //if GameController framework is available
        if ([GCController class])
        {
            [[NSNotificationCenter defaultCenter] addObserver:_connectDelegate
                                                     selector:@selector(handleControllerConnected:)
                                                         name:GCControllerDidConnectNotification
                                                       object:Nil];
            
            [[NSNotificationCenter defaultCenter] addObserver:_connectDelegate
                                                     selector:@selector(handleControllerDisconnected:)
                                                         name:GCControllerDidDisconnectNotification
                                                       object:Nil];
            
            for (GCController* controller in [GCController controllers])
            {
                handleGamepadConnected(controller);
            }
        }
    }
    
    InputApple::~InputApple()
    {
        
    }
    
    void InputApple::setCursorVisible(bool visible)
    {
#ifdef OUZEL_PLATFORM_OSX
        if (visible)
        {
            [NSCursor unhide];
        }
        else
        {
            [NSCursor hide];
        }
#endif
    }
    
    bool InputApple::isCursorVisible() const
    {
#ifdef OUZEL_PLATFORM_OSX
        return CGCursorIsVisible();
#else
        return false;
#endif
    }
    
    void InputApple::startGamepadDiscovery()
    {
        log("Started gamepad discovery");
        
        _discovering = true;
        
        if ([GCController class])
        {
            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }
    }
    
    void InputApple::stopGamepadDiscovery()
    {
        log("Stopped gamepad discovery");
        
        if (_discovering)
        {
            if ([GCController class])
            {
                [GCController stopWirelessControllerDiscovery];
            }
            
            _discovering = false;
        }
    }
    
    void InputApple::handleGamepadDiscoveryCompleted()
    {
        log("Gamepad discovery completed");
        _discovering = false;
    }
    
    void InputApple::handleGamepadConnected(id controller)
    {
        std::shared_ptr<GamepadApple> gamepad(new GamepadApple(controller));
        _gamepads.push_back(gamepad);
        
        GamepadEvent event;
        event.type = Event::Type::GAMEPAD_CONNECT;
        event.gamepad = gamepad;
        
        Engine::getInstance()->getEventDispatcher()->dispatchGamepadConnectEvent(event, Engine::getInstance()->getInput());
    }
    
    void InputApple::handleGamepadDisconnected(id controller)
    {
        std::vector<std::shared_ptr<GamepadApple>>::iterator i = std::find_if(_gamepads.begin(), _gamepads.end(), [controller](const std::shared_ptr<GamepadApple>& p) {
            return p->getController() == controller;
        });
        
        if (i != _gamepads.end())
        {
            GamepadEvent event;
            event.type = Event::Type::GAMEPAD_DISCONNECT;
            event.gamepad = *i;
            
            Engine::getInstance()->getEventDispatcher()->dispatchGamepadDisconnectEvent(event, Engine::getInstance()->getInput());
            
            _gamepads.erase(i);
        }
    }
}
