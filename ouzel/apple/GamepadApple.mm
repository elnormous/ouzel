// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <GameController/GameController.h>
#include "Engine.h"
#include "GamepadApple.h"
#include "CompileConfig.h"
#include "EventDispatcher.h"
#include "Utils.h"

namespace ouzel
{
    GamepadApple::GamepadApple(GCController* controller):
        _controller(controller)
    {
        if (_controller.extendedGamepad)
        {
            // left thumbstick
            _controller.extendedGamepad.leftThumbstick.up.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::LEFT_THUMB_UP, pressed, value);
            };
            _controller.extendedGamepad.leftThumbstick.down.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::LEFT_THUMB_DOWN, pressed, value);
            };
            _controller.extendedGamepad.leftThumbstick.left.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::LEFT_THUMB_LEFT, pressed, value);
            };
            _controller.extendedGamepad.leftThumbstick.right.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::LEFT_THUMB_RIGHT, pressed, value);
            };
            
            // right thumbstick
            _controller.extendedGamepad.rightThumbstick.up.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::RIGHT_THUMB_UP, pressed, value);
            };
            _controller.extendedGamepad.rightThumbstick.down.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::RIGHT_THUMB_DOWN, pressed, value);
            };
            _controller.extendedGamepad.rightThumbstick.left.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::RIGHT_THUMB_LEFT, pressed, value);
            };
            _controller.extendedGamepad.rightThumbstick.right.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::RIGHT_THUMB_RIGHT, pressed, value);
            };
            
            // D-pad
            _controller.extendedGamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::DPAD_UP, pressed, value);
            };
            _controller.extendedGamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::DPAD_DOWN, pressed, value);
            };
            _controller.extendedGamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::DPAD_LEFT, pressed, value);
            };
            _controller.extendedGamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::DPAD_RIGHT, pressed, value);
            };
            
            // buttons
            _controller.extendedGamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::A, pressed, value);
            };
            _controller.extendedGamepad.buttonB.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::B, pressed, value);
            };
            _controller.extendedGamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::X, pressed, value);
            };
            _controller.extendedGamepad.buttonY.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::Y, pressed, value);
            };
            
            // triggers and shoulders
            _controller.extendedGamepad.leftShoulder.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::LEFT_SHOULDER, pressed, value);
            };
            _controller.extendedGamepad.leftTrigger.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::LEFT_TRIGGER, pressed, value);
            };
            _controller.extendedGamepad.rightShoulder.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::RIGHT_SHOULDER, pressed, value);
            };
            _controller.extendedGamepad.rightTrigger.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::RIGHT_TRIGGER, pressed, value);
            };
            
        }
        else if (_controller.gamepad)
        {
            // D-pad
            _controller.gamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::DPAD_UP, pressed, value);
            };
            _controller.gamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::DPAD_DOWN, pressed, value);
            };
            _controller.gamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::DPAD_LEFT, pressed, value);
            };
            _controller.gamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::DPAD_RIGHT, pressed, value);
            };
            
            // buttons
            _controller.gamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::A, pressed, value);
            };
            _controller.gamepad.buttonB.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::B, pressed, value);
            };
            _controller.gamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::X, pressed, value);
            };
            _controller.gamepad.buttonY.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::Y, pressed, value);
            };
        }
#if defined(OUZEL_PLATFORM_TVOS)
        else if (_controller.microGamepad)
        {
            // D-pad
            _controller.microGamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::DPAD_UP, pressed, value);
            };
            _controller.microGamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::DPAD_DOWN, pressed, value);
            };
            _controller.microGamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::DPAD_LEFT, pressed, value);
            };
            _controller.microGamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::DPAD_RIGHT, pressed, value);
            };
            
            // buttons
            _controller.microGamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::A, pressed, value);
            };
            _controller.microGamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::X, pressed, value);
            };
        }
#endif
        
        _controller.controllerPausedHandler = ^(GCController* controller) {
            handleButtonValueChange(GamepadButton::PAUSE, true, 1.0f);
            handleButtonValueChange(GamepadButton::PAUSE, false, 0.0f);
        };
    }
    
    bool GamepadApple::isAttached() const
    {
        return _controller.isAttachedToDevice == YES;
    }
    
    void GamepadApple::setAbsoluteDpadValues(bool absoluteDpadValues)
    {
#if defined(OUZEL_PLATFORM_TVOS)
        _controller.microGamepad.reportsAbsoluteDpadValues = absoluteDpadValues ? YES : NO;
#endif
    }
    
    bool GamepadApple::isAbsoluteDpadValues() const
    {
#if defined(OUZEL_PLATFORM_TVOS)
        return _controller.microGamepad.reportsAbsoluteDpadValues == YES;
#else
        return false;
#endif
    }
    
    int32_t GamepadApple::getPlayerIndex() const
    {
        return static_cast<int32_t>(_controller.playerIndex);
    }
    
    bool GamepadApple::setPlayerIndex(int32_t playerIndex)
    {
        _controller.playerIndex = static_cast<GCControllerPlayerIndex>(playerIndex);
        
        return true;
    }
    
    void GamepadApple::handleButtonValueChange(GamepadButton button, bool pressed, float value)
    {
        GamepadEvent event;
        event.type = Event::Type::GAMEPAD_BUTTON_CHANGE;
        event.gamepad = shared_from_this();
        event.button = button;
        event.pressed = pressed;
        event.value = value;
        
        Engine::getInstance()->getEventDispatcher()->dispatchGamepadButtonChangeEvent(event, Engine::getInstance()->getInput());
    }
}
