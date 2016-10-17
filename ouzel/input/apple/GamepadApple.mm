// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <GameController/GameController.h>
#include <objc/message.h>
#include "core/Engine.h"
#include "GamepadApple.h"
#include "core/CompileConfig.h"
#include "events/EventDispatcher.h"
#include "utils/Log.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace input
    {
        GamepadApple::GamepadApple(GCControllerPtr aController):
            controller(aController)
        {
            if (controller.extendedGamepad)
            {
                // left thumbstick
                controller.extendedGamepad.leftThumbstick.up.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::LEFT_THUMB_UP, pressed, value);
                };
                controller.extendedGamepad.leftThumbstick.down.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::LEFT_THUMB_DOWN, pressed, value);
                };
                controller.extendedGamepad.leftThumbstick.left.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::LEFT_THUMB_LEFT, pressed, value);
                };
                controller.extendedGamepad.leftThumbstick.right.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::LEFT_THUMB_RIGHT, pressed, value);
                };

                // right thumbstick
                controller.extendedGamepad.rightThumbstick.up.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::RIGHT_THUMB_UP, pressed, value);
                };
                controller.extendedGamepad.rightThumbstick.down.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::RIGHT_THUMB_DOWN, pressed, value);
                };
                controller.extendedGamepad.rightThumbstick.left.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::RIGHT_THUMB_LEFT, pressed, value);
                };
                controller.extendedGamepad.rightThumbstick.right.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::RIGHT_THUMB_RIGHT, pressed, value);
                };

                // D-pad
                controller.extendedGamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::DPAD_UP, pressed, value);
                };
                controller.extendedGamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::DPAD_DOWN, pressed, value);
                };
                controller.extendedGamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::DPAD_LEFT, pressed, value);
                };
                controller.extendedGamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::DPAD_RIGHT, pressed, value);
                };

                // buttons
                controller.extendedGamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::A, pressed, value);
                };
                controller.extendedGamepad.buttonB.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::B, pressed, value);
                };
                controller.extendedGamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::X, pressed, value);
                };
                controller.extendedGamepad.buttonY.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::Y, pressed, value);
                };

                // triggers and shoulders
                controller.extendedGamepad.leftShoulder.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::LEFT_SHOULDER, pressed, value);
                };
                controller.extendedGamepad.leftTrigger.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::LEFT_TRIGGER, pressed, value);
                };
                controller.extendedGamepad.rightShoulder.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::RIGHT_SHOULDER, pressed, value);
                };
                controller.extendedGamepad.rightTrigger.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::RIGHT_TRIGGER, pressed, value);
                };

            }
            else if (controller.gamepad)
            {
                // D-pad
                controller.gamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::DPAD_UP, pressed, value);
                };
                controller.gamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::DPAD_DOWN, pressed, value);
                };
                controller.gamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::DPAD_LEFT, pressed, value);
                };
                controller.gamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::DPAD_RIGHT, pressed, value);
                };

                // buttons
                controller.gamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::A, pressed, value);
                };
                controller.gamepad.buttonB.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::B, pressed, value);
                };
                controller.gamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::X, pressed, value);
                };
                controller.gamepad.buttonY.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::Y, pressed, value);
                };
            }
#if OUZEL_PLATFORM_TVOS
            else if (controller.microGamepad)
            {
                // D-pad
                controller.microGamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::DPAD_UP, pressed, value);
                };
                controller.microGamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::DPAD_DOWN, pressed, value);
                };
                controller.microGamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::DPAD_LEFT, pressed, value);
                };
                controller.microGamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::DPAD_RIGHT, pressed, value);
                };

                // buttons
                controller.microGamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::A, pressed, value);
                };
                controller.microGamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput*, float value, BOOL pressed) {
                    handleButtonValueChange(GamepadButton::X, pressed, value);
                };
            }
#endif

            controller.controllerPausedHandler = ^(GCController*) {
                handleButtonValueChange(GamepadButton::PAUSE, true, 1.0f);
                handleButtonValueChange(GamepadButton::PAUSE, false, 0.0f);
            };
        }

        bool GamepadApple::isAttached() const
        {
            return controller.isAttachedToDevice == YES;
        }

        void GamepadApple::setAbsoluteDpadValues(bool absoluteDpadValues)
        {
#if OUZEL_PLATFORM_TVOS
            controller.microGamepad.reportsAbsoluteDpadValues = absoluteDpadValues ? YES : NO;
#else
            OUZEL_UNUSED(absoluteDpadValues);
#endif
        }

        bool GamepadApple::isAbsoluteDpadValues() const
        {
#if OUZEL_PLATFORM_TVOS
            return controller.microGamepad.reportsAbsoluteDpadValues == YES;
#else
            return false;
#endif
        }

        int32_t GamepadApple::getPlayerIndex() const
        {
            return static_cast<int32_t>(controller.playerIndex);
        }

        bool GamepadApple::setPlayerIndex(int32_t playerIndex)
        {
            if (playerIndex < -1 || playerIndex > 3)
            {
                Log(Log::Level::ERR) << "Invalid player index";
                return false;
            }

            controller.playerIndex = static_cast<GCControllerPlayerIndex>(playerIndex);

            return true;
        }
    } // namespace input
} // namespace ouzel
