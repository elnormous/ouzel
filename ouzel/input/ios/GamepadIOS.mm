// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "GamepadIOS.hpp"
#include "InputSystemIOS.hpp"

namespace ouzel
{
    namespace input
    {
        GamepadIOS::GamepadIOS(InputSystemIOS& initInputSystemIOS,
                               uint32_t initDeviceId,
                               GCControllerPtr initController):
            InputDevice(initDeviceId),
            inputSystemIOS(initInputSystemIOS),
            controller(initController)
        {
            if (controller.extendedGamepad)
            {
                // left thumbstick
                controller.extendedGamepad.leftThumbstick.up.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::LEFT_THUMB_UP, pressed, value);
                };
                controller.extendedGamepad.leftThumbstick.down.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::LEFT_THUMB_DOWN, pressed, value);
                };
                controller.extendedGamepad.leftThumbstick.left.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::LEFT_THUMB_LEFT, pressed, value);
                };
                controller.extendedGamepad.leftThumbstick.right.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::LEFT_THUMB_RIGHT, pressed, value);
                };

                // right thumbstick
                controller.extendedGamepad.rightThumbstick.up.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::RIGHT_THUMB_UP, pressed, value);
                };
                controller.extendedGamepad.rightThumbstick.down.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::RIGHT_THUMB_DOWN, pressed, value);
                };
                controller.extendedGamepad.rightThumbstick.left.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::RIGHT_THUMB_LEFT, pressed, value);
                };
                controller.extendedGamepad.rightThumbstick.right.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::RIGHT_THUMB_RIGHT, pressed, value);
                };

                // D-pad
                controller.extendedGamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::DPAD_UP, pressed, value);
                };
                controller.extendedGamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::DPAD_DOWN, pressed, value);
                };
                controller.extendedGamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::DPAD_LEFT, pressed, value);
                };
                controller.extendedGamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::DPAD_RIGHT, pressed, value);
                };

                // buttons
                controller.extendedGamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::FACE_BOTTOM, pressed, value);
                };
                controller.extendedGamepad.buttonB.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::FACE_RIGHT, pressed, value);
                };
                controller.extendedGamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::FACE_LEFT, pressed, value);
                };
                controller.extendedGamepad.buttonY.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::FACE_TOP, pressed, value);
                };

                // triggers and shoulders
                controller.extendedGamepad.leftShoulder.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::LEFT_SHOULDER, pressed, value);
                };
                controller.extendedGamepad.leftTrigger.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::LEFT_TRIGGER, pressed, value);
                };
                controller.extendedGamepad.rightShoulder.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::RIGHT_SHOULDER, pressed, value);
                };
                controller.extendedGamepad.rightTrigger.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::RIGHT_TRIGGER, pressed, value);
                };

            }
            else if (controller.gamepad)
            {
                // D-pad
                controller.gamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::DPAD_UP, pressed, value);
                };
                controller.gamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::DPAD_DOWN, pressed, value);
                };
                controller.gamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::DPAD_LEFT, pressed, value);
                };
                controller.gamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::DPAD_RIGHT, pressed, value);
                };

                // buttons
                controller.gamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::FACE_BOTTOM, pressed, value);
                };
                controller.gamepad.buttonB.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::FACE_RIGHT, pressed, value);
                };
                controller.gamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::FACE_LEFT, pressed, value);
                };
                controller.gamepad.buttonY.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::FACE_TOP, pressed, value);
                };
            }
#if defined(__IPHONE_10_0) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
            else if (controller.microGamepad)
            {
                // D-pad
                controller.microGamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::DPAD_UP, pressed, value);
                };
                controller.microGamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::DPAD_DOWN, pressed, value);
                };
                controller.microGamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::DPAD_LEFT, pressed, value);
                };
                controller.microGamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::DPAD_RIGHT, pressed, value);
                };

                // buttons
                controller.microGamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::FACE_BOTTOM, pressed, value);
                };
                controller.microGamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::FACE_LEFT, pressed, value);
                };
            }
#endif

            controller.controllerPausedHandler = ^(GCController*) {
                inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::PAUSE, true, 1.0F);
                inputSystemIOS.handleButtonValueChange(*this, Gamepad::Button::PAUSE, false, 0.0F);
            };

            if (controller.vendorName)
                name = [controller.vendorName cStringUsingEncoding:NSASCIIStringEncoding];

            attached = (controller.isAttachedToDevice == YES);
        }

        void GamepadIOS::setAbsoluteDPadValues(bool absoluteDPadValues)
        {
#if defined(__IPHONE_10_0) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
            controller.microGamepad.reportsAbsoluteDpadValues = absoluteDPadValues ? YES : NO;
#endif
        }

        bool GamepadIOS::isAbsoluteDPadValues() const
        {
#if defined(__IPHONE_10_0) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
            return controller.microGamepad.reportsAbsoluteDpadValues == YES;
#else
            return false;
#endif
        }

        int32_t GamepadIOS::getPlayerIndex() const
        {
            return static_cast<int32_t>(controller.playerIndex);
        }

        void GamepadIOS::setPlayerIndex(int32_t playerIndex)
        {
            switch (playerIndex)
            {
                case 0:
                    controller.playerIndex = GCControllerPlayerIndex1;
                    break;
                case 1:
                    controller.playerIndex = GCControllerPlayerIndex2;
                    break;
                case 2:
                    controller.playerIndex = GCControllerPlayerIndex3;
                    break;
                case 3:
                    controller.playerIndex = GCControllerPlayerIndex4;
                    break;
                default:
                    controller.playerIndex = GCControllerPlayerIndexUnset;
                    break;
            }
        }
    } // namespace input
} // namespace ouzel
