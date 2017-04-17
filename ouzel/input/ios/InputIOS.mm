// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#import <GameController/GameController.h>
#include "InputIOS.h"
#include "core/ios/WindowIOS.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "GamepadIOS.h"
#include "events/EventDispatcher.h"
#include "utils/Log.h"
#include "utils/Utils.h"

@interface ConnectDelegate: NSObject
{
    ouzel::input::InputIOS* input;
}

-(void)handleControllerConnected:(NSNotification*)notification;
-(void)handleControllerDisconnected:(NSNotification*)notification;

@end

@implementation ConnectDelegate

-(id)initWithInput:(ouzel::input::InputIOS*)newInput
{
    if (self = [super init])
    {
        input = newInput;
    }

    return self;
}

-(void)handleControllerConnected:(NSNotification*)notification
{
    input->handleGamepadConnected(notification.object);
}

-(void)handleControllerDisconnected:(NSNotification*)notification
{
    input->handleGamepadDisconnected(notification.object);
}

@end

namespace ouzel
{
    namespace input
    {
        InputIOS::InputIOS()
        {
        }

        bool InputIOS::init()
        {
            connectDelegate = [[ConnectDelegate alloc] initWithInput:this];

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

            return true;
        }

        void InputIOS::startGamepadDiscovery()
        {
            Log(Log::Level::INFO) << "Started gamepad discovery";

            discovering = true;

            if ([GCController class])
            {
                [GCController startWirelessControllerDiscoveryWithCompletionHandler:
                 ^(void){ handleGamepadDiscoveryCompleted(); }];
            }
        }

        void InputIOS::stopGamepadDiscovery()
        {
            Log(Log::Level::INFO) << "Stopped gamepad discovery";

            if (discovering)
            {
                if ([GCController class])
                {
                    [GCController stopWirelessControllerDiscovery];
                }

                discovering = false;
            }
        }

        bool InputIOS::showVirtualKeyboard()
        {
            UITextField* textField = static_cast<WindowIOS*>(sharedEngine->getWindow())->getTextField();
            [textField becomeFirstResponder];

            return true;
        }

        bool InputIOS::hideVirtualKeyboard()
        {
            UITextField* textField = static_cast<WindowIOS*>(sharedEngine->getWindow())->getTextField();
            [textField resignFirstResponder];

            return true;
        }

        void InputIOS::handleGamepadDiscoveryCompleted()
        {
            Log(Log::Level::INFO) << "Gamepad discovery completed";
            discovering = false;
        }

        void InputIOS::handleGamepadConnected(GCControllerPtr controller)
        {
            std::vector<int32_t> playerIndices = {0, 1, 2, 3};

            for (const auto& gamepad : gamepads)
            {
                auto i = std::find(playerIndices.begin(), playerIndices.end(), gamepad->getPlayerIndex());

                if (i != playerIndices.end()) playerIndices.erase(i);
            }

            if (!playerIndices.empty()) controller.playerIndex = static_cast<GCControllerPlayerIndex>(playerIndices.front());

            Event event;
            event.type = Event::Type::GAMEPAD_CONNECT;

            std::unique_ptr<GamepadIOS> gamepad(new GamepadIOS(controller));

            event.gamepadEvent.gamepad = gamepad.get();

            gamepads.push_back(std::move(gamepad));

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void InputIOS::handleGamepadDisconnected(GCControllerPtr controller)
        {
            std::vector<std::unique_ptr<Gamepad>>::iterator i = std::find_if(gamepads.begin(), gamepads.end(), [controller](const std::unique_ptr<Gamepad>& gamepad) {
                GamepadIOS* currentGamepad = static_cast<GamepadIOS*>(gamepad.get());
                return currentGamepad->getController() == controller;
            });

            if (i != gamepads.end())
            {
                Event event;
                event.type = Event::Type::GAMEPAD_DISCONNECT;

                event.gamepadEvent.gamepad = (*i).get();

                sharedEngine->getEventDispatcher()->postEvent(event);

                gamepads.erase(i);
            }
        }
    } // namespace input
} // namespace ouzel
