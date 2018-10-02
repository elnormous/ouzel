// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <unordered_map>
#import <UIKit/UIPress.h>
#include "InputSystemTVOS.hpp"
#include "GamepadDeviceTVOS.hpp"
#include "core/Engine.hpp"
#include "core/tvos/NativeWindowTVOS.hpp"
#include "utils/Log.hpp"

@interface ConnectDelegate: NSObject
{
    ouzel::input::InputSystemTVOS* input;
}

-(void)handleControllerConnected:(NSNotification*)notification;
-(void)handleControllerDisconnected:(NSNotification*)notification;

@end

@implementation ConnectDelegate

-(id)initWithInput:(ouzel::input::InputSystemTVOS*)initInput
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
        static const std::unordered_map<NSInteger, Keyboard::Key> keyMap = {
            {UIPressTypeUpArrow, Keyboard::Key::UP},
            {UIPressTypeDownArrow, Keyboard::Key::DOWN},
            {UIPressTypeLeftArrow, Keyboard::Key::LEFT},
            {UIPressTypeRightArrow, Keyboard::Key::RIGHT},
            {UIPressTypeSelect, Keyboard::Key::SELECT},
            {UIPressTypeMenu, Keyboard::Key::MENU},
            {UIPressTypePlayPause, Keyboard::Key::PAUSE}
        };

        Keyboard::Key InputSystemTVOS::convertKeyCode(NSInteger keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
        }

        InputSystemTVOS::InputSystemTVOS()
        {
            Event keyboardConnectEvent;
            keyboardConnectEvent.type = Event::Type::DEVICE_CONNECT;
            std::unique_ptr<KeyboardDevice> keyboard(new KeyboardDevice(*this, ++lastDeviceId));
            keyboardConnectEvent.deviceId = keyboard->getId();
            keyboardConnectEvent.deviceType = Controller::Type::KEYBOARD;
            keyboardDevice = keyboard.get();
            inputDevices.insert(std::make_pair(keyboard->getId(), std::move(keyboard)));
            addEvent(keyboardConnectEvent);

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

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        InputSystemTVOS::~InputSystemTVOS()
        {
            if (connectDelegate)
            {
                [[NSNotificationCenter defaultCenter] removeObserver:connectDelegate];

                [connectDelegate release];
            }
        }

        void InputSystemTVOS::executeCommand(Command command)
        {
            switch (command.type)
            {
                case Command::Type::START_DEVICE_DISCOVERY:
                    startGamepadDiscovery();
                    break;
                case Command::Type::STOP_DEVICE_DISCOVERY:
                    stopGamepadDiscovery();
                    break;
                case Command::Type::SET_ABSOLUTE_DPAD_VALUES:
                {
                    break;
                }
                case Command::Type::SET_PLAYER_INDEX:
                {
                    break;
                }
                case Command::Type::SET_VIBRATION:
                {
                    break;
                }
                case Command::Type::SET_POSITON:
                {
                    break;
                }
                case Command::Type::SET_CURSOR:
                {
                    break;
                }
                case Command::Type::SHOW_VIRTUAL_KEYBOARD:
                    showVirtualKeyboard();
                    break;
                case Command::Type::HIDE_VIRTUAL_KEYBOARD:
                    hideVirtualKeyboard();
                    break;
            }
        }

        void InputSystemTVOS::startGamepadDiscovery()
        {
            Log(Log::Level::INFO) << "Started gamepad discovery";

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        void InputSystemTVOS::stopGamepadDiscovery()
        {
            Log(Log::Level::INFO) << "Stopped gamepad discovery";

            [GCController stopWirelessControllerDiscovery];
        }

        void InputSystemTVOS::handleGamepadDiscoveryCompleted()
        {
            Log(Log::Level::INFO) << "Gamepad discovery completed";
        }

        void InputSystemTVOS::handleGamepadConnected(GCControllerPtr controller)
        {
            /*std::vector<int32_t> playerIndices = {0, 1, 2, 3};

            for (const auto& gamepad : gamepads)
            {
                auto i = std::find(playerIndices.begin(), playerIndices.end(), gamepad->getPlayerIndex());

                if (i != playerIndices.end()) playerIndices.erase(i);
            }

            if (!playerIndices.empty()) controller.playerIndex = static_cast<GCControllerPlayerIndex>(playerIndices.front());

            Event event;
            event.type = Event::Type::GAMEPAD_CONNECT;

            std::unique_ptr<GamepadDeviceTVOS> gamepad(new GamepadDeviceTVOS(controller));

             event.gamepadEvent.gamepad = gamepad.get();

             gamepads.push_back(std::move(gamepad));*/
        }

        void InputSystemTVOS::handleGamepadDisconnected(GCControllerPtr controller)
        {
            /*auto i = std::find_if(gamepads.begin(), gamepads.end(), [controller](const std::unique_ptr<Gamepad>& gamepad) {
             GamepadDeviceTVOS* currentGamepad = static_cast<GamepadDeviceTVOS*>(gamepad.get());
             return currentGamepad->getController() == controller;
             });

             if (i != gamepads.end())
             {
             Event event;
             event.type = Event::Type::GAMEPAD_DISCONNECT;

             event.gamepadEvent.gamepad = (*i).get();

             engine->getEventDispatcher().postEvent(event);

             gamepads.erase(i);
             }*/
        }

        void InputSystemTVOS::showVirtualKeyboard()
        {
            NativeWindowTVOS* windowTVOS = static_cast<NativeWindowTVOS*>(engine->getWindow()->getNativeWindow());
            UITextField* textField = windowTVOS->getTextField();
            [textField becomeFirstResponder];
        }

        void InputSystemTVOS::hideVirtualKeyboard()
        {
            NativeWindowTVOS* windowTVOS = static_cast<NativeWindowTVOS*>(engine->getWindow()->getNativeWindow());
            UITextField* textField = windowTVOS->getTextField();
            [textField resignFirstResponder];
        }
    } // namespace input
} // namespace ouzel
