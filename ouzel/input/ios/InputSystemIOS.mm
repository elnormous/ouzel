// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <unordered_map>
#import <UIKit/UIPress.h>
#include "InputSystemIOS.hpp"
#include "GamepadDeviceIOS.hpp"
#include "core/Engine.hpp"
#include "core/ios/NativeWindowIOS.hpp"
#include "utils/Log.hpp"

@interface ConnectDelegate: NSObject
{
    ouzel::input::InputSystemIOS* input;
}

-(void)handleControllerConnected:(NSNotification*)notification;
-(void)handleControllerDisconnected:(NSNotification*)notification;

@end

@implementation ConnectDelegate

-(id)initWithInput:(ouzel::input::InputSystemIOS*)initInput
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

        Keyboard::Key InputSystemIOS::convertKeyCode(NSInteger keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
        }

        InputSystemIOS::InputSystemIOS(EventHandler& initEventHandler):
            InputSystem(initEventHandler),
            keyboardDevice(new KeyboardDevice(*this, ++lastDeviceId)),
            touchpadDevice(new TouchpadDevice(*this, ++lastDeviceId))
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

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        InputSystemIOS::~InputSystemIOS()
        {
            if (connectDelegate)
            {
                [[NSNotificationCenter defaultCenter] removeObserver:connectDelegate];

                [connectDelegate release];
            }
        }

        void InputSystemIOS::executeCommand(const Command& command)
        {
            switch (command.type)
            {
                case Command::Type::START_DEVICE_DISCOVERY:
                    startGamepadDiscovery();
                    break;
                case Command::Type::STOP_DEVICE_DISCOVERY:
                    stopGamepadDiscovery();
                    break;
                case Command::Type::SET_PLAYER_INDEX:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        GamepadDeviceIOS* gamepadDevice = static_cast<GamepadDeviceIOS*>(inputDevice);
                        gamepadDevice->setPlayerIndex(command.playerIndex);
                    }
                    break;
                }
                case Command::Type::SET_VIBRATION:
                {
                    break;
                }
                case Command::Type::SHOW_VIRTUAL_KEYBOARD:
                    showVirtualKeyboard();
                    break;
                case Command::Type::HIDE_VIRTUAL_KEYBOARD:
                    hideVirtualKeyboard();
                    break;
                default:
                    break;
            }
        }

        void InputSystemIOS::startGamepadDiscovery()
        {
            Log(Log::Level::INFO) << "Started gamepad discovery";

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        void InputSystemIOS::stopGamepadDiscovery()
        {
            Log(Log::Level::INFO) << "Stopped gamepad discovery";

            [GCController stopWirelessControllerDiscovery];
        }

        void InputSystemIOS::handleGamepadDiscoveryCompleted()
        {
            Log(Log::Level::INFO) << "Gamepad discovery completed";
        }

        void InputSystemIOS::handleGamepadConnected(GCControllerPtr controller)
        {
            std::vector<int32_t> playerIndices = {0, 1, 2, 3};

            for (const auto& i : gamepadDevices)
            {
                auto n = std::find(playerIndices.begin(), playerIndices.end(), i.second->getPlayerIndex());
                if (n != playerIndices.end()) playerIndices.erase(n);
            }

            if (!playerIndices.empty()) controller.playerIndex = static_cast<GCControllerPlayerIndex>(playerIndices.front());

            std::unique_ptr<GamepadDeviceIOS> gamepadDevice(new GamepadDeviceIOS(*this, ++lastDeviceId, controller));
            gamepadDevices.insert(std::make_pair(controller, std::move(gamepadDevice)));
        }

        void InputSystemIOS::handleGamepadDisconnected(GCControllerPtr controller)
        {
            auto i = gamepadDevices.find(controller);

            if (i != gamepadDevices.end())
                gamepadDevices.erase(i);
        }

        void InputSystemIOS::showVirtualKeyboard()
        {
            NativeWindowIOS* windowIOS = static_cast<NativeWindowIOS*>(engine->getWindow()->getNativeWindow());
            UITextField* textField = windowIOS->getTextField();
            [textField becomeFirstResponder];
        }

        void InputSystemIOS::hideVirtualKeyboard()
        {
            NativeWindowIOS* windowIOS = static_cast<NativeWindowIOS*>(engine->getWindow()->getNativeWindow());
            UITextField* textField = windowIOS->getTextField();
            [textField resignFirstResponder];
        }
    } // namespace input
} // namespace ouzel
