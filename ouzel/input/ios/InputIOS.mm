// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <unordered_map>
#import <GameController/GameController.h>
#include "InputIOS.hpp"
#include "GamepadIOS.hpp"
#include "core/ios/WindowResourceIOS.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "utils/Log.hpp"

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
        static const std::unordered_map<NSInteger, KeyboardKey> keyMap = {
            {UIPressTypeUpArrow, KeyboardKey::UP},
            {UIPressTypeDownArrow, KeyboardKey::DOWN},
            {UIPressTypeLeftArrow, KeyboardKey::LEFT},
            {UIPressTypeRightArrow, KeyboardKey::RIGHT},
            {UIPressTypeSelect, KeyboardKey::SELECT},
            {UIPressTypeMenu, KeyboardKey::MENU},
            {UIPressTypePlayPause, KeyboardKey::PAUSE}
        };

        KeyboardKey InputIOS::convertKeyCode(NSInteger keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
            {
                return i->second;
            }
            else
            {
                return KeyboardKey::NONE;
            }
        }

        InputIOS::InputIOS()
        {
        }

        InputIOS::~InputIOS()
        {
            if (connectDelegate)
            {
                [[NSNotificationCenter defaultCenter] removeObserver:connectDelegate];

                [connectDelegate release];
            }
        }

        bool InputIOS::init()
        {
            connectDelegate = [[ConnectDelegate alloc] initWithInput:this];

            [[NSNotificationCenter defaultCenter] addObserver:connectDelegate
                                                     selector:@selector(handleControllerConnected:)
                                                         name:GCControllerDidConnectNotification
                                                       object:nil];

            [[NSNotificationCenter defaultCenter] addObserver:connectDelegate
                                                     selector:@selector(handleControllerDisconnected:)
                                                         name:GCControllerDidDisconnectNotification
                                                       object:nil];

            for (GCController* controller in [GCController controllers])
            {
                handleGamepadConnected(controller);
            }

            startGamepadDiscovery();

            return true;
        }

        void InputIOS::startGamepadDiscovery()
        {
            Log(Log::Level::INFO) << "Started gamepad discovery";

            discovering = true;

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        void InputIOS::stopGamepadDiscovery()
        {
            if (discovering)
            {
                Log(Log::Level::INFO) << "Stopped gamepad discovery";

                [GCController stopWirelessControllerDiscovery];

                discovering = false;
            }
        }

        bool InputIOS::showVirtualKeyboard()
        {
            engine->executeOnMainThread([]() {
                WindowResourceIOS* windowIOS = static_cast<WindowResourceIOS*>(engine->getWindow()->getResource());
                UITextField* textField = windowIOS->getTextField();
                [textField becomeFirstResponder];
            });

            return true;
        }

        bool InputIOS::hideVirtualKeyboard()
        {
            engine->executeOnMainThread([]() {
                WindowResourceIOS* windowIOS = static_cast<WindowResourceIOS*>(engine->getWindow()->getResource());
                UITextField* textField = windowIOS->getTextField();
                [textField resignFirstResponder];
            });

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

            engine->getEventDispatcher()->postEvent(event);
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

                engine->getEventDispatcher()->postEvent(event);

                gamepads.erase(i);
            }
        }
    } // namespace input
} // namespace ouzel
