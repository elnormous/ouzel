// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "InputSystemIOS.hpp"
#include "GamepadDeviceIOS.hpp"
#include "../../core/Engine.hpp"
#include "../../core/ios/NativeWindowIOS.hpp"
#include "../../utils/Log.hpp"

@interface ConnectDelegate: NSObject

@end

@implementation ConnectDelegate
{
    ouzel::input::InputSystemIOS* input;
}

- (id)initWithInput:(ouzel::input::InputSystemIOS*)initInput
{
    if (self = [super init])
        input = initInput;

    return self;
}

- (void)handleControllerConnected:(NSNotification*)notification
{
    input->handleGamepadConnected(notification.object);
}

- (void)handleControllerDisconnected:(NSNotification*)notification
{
    input->handleGamepadDisconnected(notification.object);
}
@end

namespace ouzel
{
    namespace input
    {
        InputSystemIOS::InputSystemIOS(const std::function<std::future<bool>(const Event&)>& initCallback):
            InputSystem(initCallback),
            keyboardDevice(std::make_unique<KeyboardDevice>(*this, getNextDeviceId())),
            touchpadDevice(std::make_unique<TouchpadDevice>(*this, getNextDeviceId(), true))
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
                case Command::Type::startDeviceDiscovery:
                    startGamepadDiscovery();
                    break;
                case Command::Type::stopDeviceDiscovery:
                    stopGamepadDiscovery();
                    break;
                case Command::Type::setPlayerIndex:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        auto gamepadDevice = static_cast<GamepadDeviceIOS*>(inputDevice);
                        gamepadDevice->setPlayerIndex(command.playerIndex);
                    }
                    break;
                }
                case Command::Type::setVibration:
                {
                    break;
                }
                case Command::Type::showVirtualKeyboard:
                    showVirtualKeyboard();
                    break;
                case Command::Type::hideVirtualKeyboard:
                    hideVirtualKeyboard();
                    break;
                default:
                    break;
            }
        }

        void InputSystemIOS::startGamepadDiscovery()
        {
            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        void InputSystemIOS::stopGamepadDiscovery()
        {
            [GCController stopWirelessControllerDiscovery];
        }

        void InputSystemIOS::handleGamepadDiscoveryCompleted()
        {
            sendEvent(Event(Event::Type::deviceDiscoveryComplete));
        }

        void InputSystemIOS::handleGamepadConnected(GCControllerPtr controller)
        {
            std::vector<std::int32_t> playerIndices = {0, 1, 2, 3};

            for (const auto& i : gamepadDevices)
            {
                auto n = std::find(playerIndices.begin(), playerIndices.end(), i.second->getPlayerIndex());
                if (n != playerIndices.end()) playerIndices.erase(n);
            }

            if (!playerIndices.empty()) controller.playerIndex = static_cast<GCControllerPlayerIndex>(playerIndices.front());

            auto gamepadDevice = std::make_unique<GamepadDeviceIOS>(*this, getNextDeviceId(), controller);
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
            auto windowIOS = static_cast<NativeWindowIOS*>(engine->getWindow()->getNativeWindow());
            UITextField* textField = windowIOS->getTextField();
            [textField becomeFirstResponder];
        }

        void InputSystemIOS::hideVirtualKeyboard()
        {
            auto windowIOS = static_cast<NativeWindowIOS*>(engine->getWindow()->getNativeWindow());
            UITextField* textField = windowIOS->getTextField();
            [textField resignFirstResponder];
        }
    } // namespace input
} // namespace ouzel
