// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "ViewTVOS.h"
#include "core/Engine.hpp"
#include "input/tvos/InputManagerTVOS.hpp"

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
    for (UIPress* press in presses)
        ouzel::engine->getInputManager()->keyPress(ouzel::input::InputManagerTVOS::convertKeyCode(press.type), 0);
}

-(void)pressesEnded:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    for (UIPress* press in presses)
        ouzel::engine->getInputManager()->keyRelease(ouzel::input::InputManagerTVOS::convertKeyCode(press.type), 0);
}

-(void)pressesCancelled:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    for (UIPress* press in presses)
        ouzel::engine->getInputManager()->keyRelease(ouzel::input::InputManagerTVOS::convertKeyCode(press.type), 0);
}

@end
