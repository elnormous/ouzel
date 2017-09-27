// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "ViewIOS.h"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "input/ios/InputIOS.hpp"

@implementation ViewIOS

-(void)touchesBegan:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        ouzel::sharedEngine->getInput()->touchBegin(reinterpret_cast<uint64_t>(touch),
                                                    ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                       static_cast<float>(location.y))),
                                                    (touch.maximumPossibleForce > 0.0f) ? static_cast<float>(touch.force / touch.maximumPossibleForce) : 0.0f);
    }
}

-(void)touchesMoved:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        ouzel::sharedEngine->getInput()->touchMove(reinterpret_cast<uint64_t>(touch),
                                                   ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                      static_cast<float>(location.y))),
                                                   (touch.maximumPossibleForce > 0.0f) ? static_cast<float>(touch.force / touch.maximumPossibleForce) : 0.0f);
    }
}

-(void)touchesEnded:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        ouzel::sharedEngine->getInput()->touchEnd(reinterpret_cast<uint64_t>(touch),
                                                  ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                     static_cast<float>(location.y))),
                                                  (touch.maximumPossibleForce > 0.0f) ? static_cast<float>(touch.force / touch.maximumPossibleForce) : 0.0f);
    }
}

-(void)touchesCancelled:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        ouzel::sharedEngine->getInput()->touchCancel(reinterpret_cast<uint64_t>(touch),
                                                     ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                        static_cast<float>(location.y))),
                                                     (touch.maximumPossibleForce > 0.0f) ? static_cast<float>(touch.force / touch.maximumPossibleForce) : 0.0f);
    }
}

-(void)pressesBegan:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    for (UIPress* press in presses)
    {
        ouzel::sharedEngine->getInput()->keyPress(ouzel::input::InputIOS::convertKeyCode(press.type), 0);
    }
}

-(void)pressesEnded:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    for (UIPress* press in presses)
    {
        ouzel::sharedEngine->getInput()->keyRelease(ouzel::input::InputIOS::convertKeyCode(press.type), 0);
    }
}

-(void)pressesCancelled:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    for (UIPress* press in presses)
    {
        ouzel::sharedEngine->getInput()->keyRelease(ouzel::input::InputIOS::convertKeyCode(press.type), 0);
    }
}

@end
