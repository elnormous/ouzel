// Copyright (C) 2016 Elviss Strazdins
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
    std::shared_ptr<ouzel::input::InputApple> inputApple = std::static_pointer_cast<ouzel::input::InputApple>(ouzel::sharedEngine->getInput());
    inputApple->handleGamepadConnected(notification.object);
}

-(void)handleControllerDisconnected:(NSNotification*)notification
{
    std::shared_ptr<ouzel::input::InputApple> inputApple = std::static_pointer_cast<ouzel::input::InputApple>(ouzel::sharedEngine->getInput());
    inputApple->handleGamepadDisconnected(notification.object);
}

@end

namespace ouzel
{
    namespace input
    {
        InputApple::InputApple()
        {
            connectDelegate = [[ConnectDelegate alloc]init];

            //if GameController framework is available
            if ([GCController class])
            {
                [[NSNotificationCenter defaultCenter] addObserver:connectDelegate
                                                         selector:@selector(handleControllerConnected:)
                                                             name:GCControllerDidConnectNotification
                                                           object:Nil];

                [[NSNotificationCenter defaultCenter] addObserver:connectDelegate
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
#else
            OUZEL_UNUSED(visible);
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

            discovering = true;

            if ([GCController class])
            {
                [GCController startWirelessControllerDiscoveryWithCompletionHandler:
                 ^(void){ handleGamepadDiscoveryCompleted(); }];
            }
        }

        void InputApple::stopGamepadDiscovery()
        {
            log("Stopped gamepad discovery");

            if (discovering)
            {
                if ([GCController class])
                {
                    [GCController stopWirelessControllerDiscovery];
                }

                discovering = false;
            }
        }

        void InputApple::handleGamepadDiscoveryCompleted()
        {
            log("Gamepad discovery completed");
            discovering = false;
        }

        void InputApple::handleGamepadConnected(id controller)
        {
            std::shared_ptr<GamepadApple> gamepad(new GamepadApple(controller));
            gamepads.push_back(gamepad);

            GamepadEventPtr event = std::make_shared<GamepadEvent>();
            event->type = Event::Type::GAMEPAD_CONNECT;
            event->gamepad = gamepad;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, sharedEngine->getInput());
        }

        void InputApple::handleGamepadDisconnected(id controller)
        {
            std::vector<std::shared_ptr<GamepadApple>>::iterator i = std::find_if(gamepads.begin(), gamepads.end(), [controller](const std::shared_ptr<GamepadApple>& p) {
                return p->getController() == controller;
            });

            if (i != gamepads.end())
            {
                GamepadEventPtr event = std::make_shared<GamepadEvent>();
                event->type = Event::Type::GAMEPAD_DISCONNECT;
                event->gamepad = *i;

                sharedEngine->getEventDispatcher()->dispatchEvent(event, sharedEngine->getInput());

                gamepads.erase(i);
            }
        }
    } // namespace input
} // namespace ouzel
