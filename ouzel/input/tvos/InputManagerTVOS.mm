// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <unordered_map>
#import <GameController/GameController.h>
#include "InputManagerTVOS.hpp"
#include "GamepadTVOS.hpp"
#include "core/tvos/NativeWindowTVOS.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "utils/Log.hpp"

@interface ConnectDelegate: NSObject
{
    ouzel::input::InputManagerTVOS* input;
}

-(void)handleControllerConnected:(NSNotification*)notification;
-(void)handleControllerDisconnected:(NSNotification*)notification;

@end

@implementation ConnectDelegate

-(id)initWithInput:(ouzel::input::InputManagerTVOS*)initInput
{
    if (self = [super init])
        input = initInput;

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

        KeyboardKey InputManagerTVOS::convertKeyCode(NSInteger keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return KeyboardKey::NONE;
        }

        InputManagerTVOS::InputManagerTVOS()
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
                handleGamepadConnected(controller);

            discovering = true;

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        InputManagerTVOS::~InputManagerTVOS()
        {
            if (connectDelegate)
            {
                [[NSNotificationCenter defaultCenter] removeObserver:connectDelegate];

                [connectDelegate release];
            }
        }

        void InputManagerTVOS::startGamepadDiscovery()
        {
            Log(Log::Level::INFO) << "Started gamepad discovery";

            discovering = true;

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        void InputManagerTVOS::stopGamepadDiscovery()
        {
            if (discovering)
            {
                Log(Log::Level::INFO) << "Stopped gamepad discovery";

                [GCController stopWirelessControllerDiscovery];

                discovering = false;
            }
        }

        void InputManagerTVOS::showVirtualKeyboard()
        {
            engine->executeOnMainThread([]() {
                NativeWindowTVOS* windowTVOS = static_cast<NativeWindowTVOS*>(engine->getWindow()->getNativeWindow());
                UITextField* textField = windowTVOS->getTextField();
                [textField becomeFirstResponder];
            });
        }

        void InputManagerTVOS::hideVirtualKeyboard()
        {
            engine->executeOnMainThread([]() {
                NativeWindowTVOS* windowTVOS = static_cast<NativeWindowTVOS*>(engine->getWindow()->getNativeWindow());
                UITextField* textField = windowTVOS->getTextField();
                [textField resignFirstResponder];
            });
        }

        void InputManagerTVOS::handleGamepadDiscoveryCompleted()
        {
            Log(Log::Level::INFO) << "Gamepad discovery completed";
            discovering = false;
        }

        void InputManagerTVOS::handleGamepadConnected(GCControllerPtr controller)
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

            std::unique_ptr<GamepadTVOS> gamepad(new GamepadTVOS(controller));

            event.gamepadEvent.gamepad = gamepad.get();

            gamepads.push_back(std::move(gamepad));

            engine->getEventDispatcher()->postEvent(event);
        }

        void InputManagerTVOS::handleGamepadDisconnected(GCControllerPtr controller)
        {
            auto i = std::find_if(gamepads.begin(), gamepads.end(), [controller](const std::unique_ptr<Gamepad>& gamepad) {
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
