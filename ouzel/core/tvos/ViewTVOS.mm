// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "ViewTVOS.h"
#include "core/Engine.h"
#include "input/tvos/InputTVOS.h"

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
    {
        ouzel::sharedEngine->getInput()->keyPress(ouzel::input::InputTVOS::convertKeyCode(press.type), 0);
    }
}

-(void)pressesEnded:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    for (UIPress* press in presses)
    {
        ouzel::sharedEngine->getInput()->keyRelease(ouzel::input::InputTVOS::convertKeyCode(press.type), 0);
    }
}

-(void)pressesCancelled:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    for (UIPress* press in presses)
    {
        ouzel::sharedEngine->getInput()->keyRelease(ouzel::input::InputTVOS::convertKeyCode(press.type), 0);
    }
}

@end
