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
            _controller.extendedGamepad.leftThumbstick.valueChangedHandler = ^(GCControllerDirectionPad* dpad, float xValue, float yValue) {
                
            };
        }
        else if (_controller.gamepad)
        {
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
            
            _controller.microGamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::A, pressed, value);
            };
            _controller.microGamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                handleButtonValueChange(GamepadButton::X, pressed, value);
            };
        }
#endif
        
        _controller.controllerPausedHandler = ^(GCController* controller) {
            log("controllerPausedHandler");
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
    
    void GamepadApple::setPlayerIndex(int32_t playerIndex)
    {
        _controller.playerIndex = static_cast<GCControllerPlayerIndex>(playerIndex);
    }
    
    void GamepadApple::handleButtonValueChange(GamepadButton button, bool pressed, float value)
    {
        GamepadEvent event;
        event.type = Event::Type::GAMEPAD_BUTTON_CHANGE;
        event.button = button;
        event.pressed = pressed;
        event.value = value;
        
        Engine::getInstance()->getEventDispatcher()->dispatchGamepadButtonChangeEvent(event, Engine::getInstance()->getInput());
    }
}
