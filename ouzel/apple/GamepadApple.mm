// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <GameController/GameController.h>
#include "GamepadApple.h"
#include "CompileConfig.h"
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
            _controller.gamepad.dpad.valueChangedHandler = ^(GCControllerDirectionPad* dpad, float xValue, float yValue) {
                
            };
            
            _controller.gamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                
            };
            _controller.gamepad.buttonB.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                
            };
            _controller.gamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                
            };
            _controller.gamepad.buttonY.valueChangedHandler = ^(GCControllerButtonInput* button, float value, BOOL pressed) {
                
            };
        }
#if defined(OUZEL_PLATFORM_TVOS)
        else if (_controller.microGamepad)
        {
            _controller.microGamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                log("dpad.up.valueChangedHandler");
            };
            
            _controller.microGamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                log("dpad.down.valueChangedHandler");
            };
            
            _controller.microGamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                log("dpad.left.valueChangedHandler");
            };
            
            _controller.microGamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                log("dpad.right.valueChangedHandler");
            };
            
            _controller.microGamepad.valueChangedHandler = ^(GCMicroGamepad *gamepad, GCControllerElement *element) {
                
                if (element == gamepad.buttonA)
                {
                    log("gamepad.buttonA");
                }
                else if (element == gamepad.buttonX)
                {
                    log("gamepad.buttonX");
                }
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
}
