// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <unordered_map>
#import <GameController/GameController.h>
#include "InputTVOS.hpp"
#include "GamepadTVOS.hpp"
#include "core/tvos/WindowResourceTVOS.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "utils/Log.hpp"

@interface ConnectDelegate: NSObject
{
    ouzel::input::InputTVOS* input;
}

-(void)handleControllerConnected:(NSNotification*)notification;
-(void)handleControllerDisconnected:(NSNotification*)notification;

@end

@implementation ConnectDelegate

-(id)initWithInput:(ouzel::input::InputTVOS*)newInput
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

        KeyboardKey InputTVOS::convertKeyCode(NSInteger keyCode)
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

        InputTVOS::InputTVOS()
        {
        }

        InputTVOS::~InputTVOS()
        {
            if (connectDelegate)
            {
                [[NSNotificationCenter defaultCenter] removeObserver:connectDelegate];

                [connectDelegate release];
            }
        }

        bool InputTVOS::init()
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

        void InputTVOS::startGamepadDiscovery()
        {
            Log(Log::Level::INFO) << "Started gamepad discovery";

            discovering = true;

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        void InputTVOS::stopGamepadDiscovery()
        {
            if (discovering)
            {
                Log(Log::Level::INFO) << "Stopped gamepad discovery";

                [GCController stopWirelessControllerDiscovery];

                discovering = false;
            }
        }

        bool InputTVOS::showVirtualKeyboard()
        {
            engine->executeOnMainThread([]() {
                WindowResourceTVOS* windowTVOS = static_cast<WindowResourceTVOS*>(engine->getWindow()->getResource());
                UITextField* textField = windowTVOS->getTextField();
                [textField becomeFirstResponder];
            });

            return true;
        }

        bool InputTVOS::hideVirtualKeyboard()
        {
            engine->executeOnMainThread([]() {
                WindowResourceTVOS* windowTVOS = static_cast<WindowResourceTVOS*>(engine->getWindow()->getResource());
                UITextField* textField = windowTVOS->getTextField();
                [textField resignFirstResponder];
            });

            return true;
        }

        void InputTVOS::handleGamepadDiscoveryCompleted()
        {
            Log(Log::Level::INFO) << "Gamepad discovery completed";
            discovering = false;
        }

        void InputTVOS::handleGamepadConnected(GCControllerPtr controller)
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

            UniquePtr<GamepadTVOS> gamepad(new GamepadTVOS(controller));

            event.gamepadEvent.gamepad = gamepad.get();

            gamepads.push_back(std::move(gamepad));

            engine->getEventDispatcher()->postEvent(event);
        }

        void InputTVOS::handleGamepadDisconnected(GCControllerPtr controller)
        {
            std::vector<UniquePtr<Gamepad>>::iterator i = std::find_if(gamepads.begin(), gamepads.end(), [controller](const UniquePtr<Gamepad>& gamepad) {
                GamepadTVOS* currentGamepad = static_cast<GamepadTVOS*>(gamepad.get());
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
