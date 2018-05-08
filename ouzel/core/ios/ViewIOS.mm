// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "ViewIOS.h"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "input/ios/InputManagerIOS.hpp"

@implementation ViewIOS

-(void)touchesBegan:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        CGFloat force = 1.0;
        if ([touch respondsToSelector:@selector(force)] && [touch respondsToSelector:@selector(maximumPossibleForce)])
            force = (touch.maximumPossibleForce > 0.0F) ? touch.force / touch.maximumPossibleForce : 0.0F;

        ouzel::engine->getInputManager()->touchBegin(reinterpret_cast<uint64_t>(touch),
                                                     ouzel::Vector2(static_cast<float>(location.x / self.bounds.size.width),
                                                                    static_cast<float>(location.y / self.bounds.size.height)),
                                                     static_cast<float>(force));
    }
}

-(void)touchesMoved:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        CGFloat force = 1.0;
        if ([touch respondsToSelector:@selector(force)] && [touch respondsToSelector:@selector(maximumPossibleForce)])
            force = (touch.maximumPossibleForce > 0.0F) ? touch.force / touch.maximumPossibleForce : 0.0F;

        ouzel::engine->getInputManager()->touchMove(reinterpret_cast<uint64_t>(touch),
                                                    ouzel::Vector2(static_cast<float>(location.x / self.bounds.size.width),
                                                                   static_cast<float>(location.y / self.bounds.size.height)),
                                                    static_cast<float>(force));
    }
}

-(void)touchesEnded:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        CGFloat force = 1.0;
        if ([touch respondsToSelector:@selector(force)] && [touch respondsToSelector:@selector(maximumPossibleForce)])
            force = (touch.maximumPossibleForce > 0.0F) ? touch.force / touch.maximumPossibleForce : 0.0F;

        ouzel::engine->getInputManager()->touchEnd(reinterpret_cast<uint64_t>(touch),
                                                   ouzel::Vector2(static_cast<float>(location.x / self.bounds.size.width),
                                                                  static_cast<float>(location.y / self.bounds.size.height)),
                                                   static_cast<float>(force));
    }
}

-(void)touchesCancelled:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        CGFloat force = 1.0;
        if ([touch respondsToSelector:@selector(force)] && [touch respondsToSelector:@selector(maximumPossibleForce)])
            force = (touch.maximumPossibleForce > 0.0F) ? touch.force / touch.maximumPossibleForce : 0.0F;

        ouzel::engine->getInputManager()->touchCancel(reinterpret_cast<uint64_t>(touch),
                                                      ouzel::Vector2(static_cast<float>(location.x / self.bounds.size.width),
                                                                     static_cast<float>(location.y / self.bounds.size.height)),
                                                      static_cast<float>(force));
    }
}

-(void)pressesBegan:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    for (UIPress* press in presses)
    {
        ouzel::engine->getInputManager()->keyPress(ouzel::input::InputManagerIOS::convertKeyCode(press.type), 0);
    }
}

-(void)pressesEnded:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    for (UIPress* press in presses)
    {
        ouzel::engine->getInputManager()->keyRelease(ouzel::input::InputManagerIOS::convertKeyCode(press.type), 0);
    }
}

-(void)pressesCancelled:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    for (UIPress* press in presses)
    {
        ouzel::engine->getInputManager()->keyRelease(ouzel::input::InputManagerIOS::convertKeyCode(press.type), 0);
    }
}

@end
