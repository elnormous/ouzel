// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#import "ViewTVOS.h"
#include "core/Engine.hpp"
#include "input/tvos/InputSystemTVOS.hpp"

@implementation ViewTVOS

-(void)insertText:(__unused NSString*)text
{
}

-(void)deleteBackward
{
}

-(BOOL)hasText
{
    return NO;
}

-(BOOL)canBecomeFirstResponder
{
    return YES;
}

-(void)pressesBegan:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    ouzel::input::InputSystemTVOS* inputSystemTVOS = static_cast<ouzel::input::InputSystemTVOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemTVOS->getKeyboardDevice();
    for (UIPress* press in presses)
        keyboardDevice->handleKeyPress(ouzel::input::InputSystemTVOS::convertKeyCode(press.type), 0);
}

-(void)pressesEnded:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    ouzel::input::InputSystemTVOS* inputSystemTVOS = static_cast<ouzel::input::InputSystemTVOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemTVOS->getKeyboardDevice();
    for (UIPress* press in presses)
        keyboardDevice->handleKeyRelease(ouzel::input::InputSystemTVOS::convertKeyCode(press.type), 0);
}

-(void)pressesCancelled:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    ouzel::input::InputSystemTVOS* inputSystemTVOS = static_cast<ouzel::input::InputSystemTVOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemTVOS->getKeyboardDevice();
    for (UIPress* press in presses)
        keyboardDevice->handleKeyRelease(ouzel::input::InputSystemTVOS::convertKeyCode(press.type), 0);
}

@end
