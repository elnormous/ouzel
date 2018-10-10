// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#import "ViewIOS.h"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "input/ios/InputSystemIOS.hpp"

@implementation ViewIOS

-(void)touchesBegan:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    ouzel::input::InputSystemIOS* inputSystemIOS = static_cast<ouzel::input::InputSystemIOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDevice* touchpadDevice = inputSystemIOS->getTouchpadDevice();
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        CGFloat force = 1.0;
        if ([touch respondsToSelector:@selector(force)] && [touch respondsToSelector:@selector(maximumPossibleForce)])
            force = (touch.maximumPossibleForce > 0.0F) ? touch.force / touch.maximumPossibleForce : 0.0F;

        touchpadDevice->handleTouchBegin(reinterpret_cast<uint64_t>(touch),
                                         ouzel::Vector2(static_cast<float>(location.x / self.bounds.size.width),
                                                        static_cast<float>(location.y / self.bounds.size.height)),
                                         static_cast<float>(force));
    }
}

-(void)touchesMoved:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    ouzel::input::InputSystemIOS* inputSystemIOS = static_cast<ouzel::input::InputSystemIOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDevice* touchpadDevice = inputSystemIOS->getTouchpadDevice();
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        CGFloat force = 1.0;
        if ([touch respondsToSelector:@selector(force)] && [touch respondsToSelector:@selector(maximumPossibleForce)])
            force = (touch.maximumPossibleForce > 0.0F) ? touch.force / touch.maximumPossibleForce : 0.0F;

        touchpadDevice->handleTouchMove(reinterpret_cast<uint64_t>(touch),
                                        ouzel::Vector2(static_cast<float>(location.x / self.bounds.size.width),
                                                       static_cast<float>(location.y / self.bounds.size.height)),
                                        static_cast<float>(force));
    }
}

-(void)touchesEnded:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    ouzel::input::InputSystemIOS* inputSystemIOS = static_cast<ouzel::input::InputSystemIOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDevice* touchpadDevice = inputSystemIOS->getTouchpadDevice();
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        CGFloat force = 1.0;
        if ([touch respondsToSelector:@selector(force)] && [touch respondsToSelector:@selector(maximumPossibleForce)])
            force = (touch.maximumPossibleForce > 0.0F) ? touch.force / touch.maximumPossibleForce : 0.0F;

        touchpadDevice->handleTouchEnd(reinterpret_cast<uint64_t>(touch),
                                       ouzel::Vector2(static_cast<float>(location.x / self.bounds.size.width),
                                                      static_cast<float>(location.y / self.bounds.size.height)),
                                       static_cast<float>(force));
    }
}

-(void)touchesCancelled:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    ouzel::input::InputSystemIOS* inputSystemIOS = static_cast<ouzel::input::InputSystemIOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDevice* touchpadDevice = inputSystemIOS->getTouchpadDevice();
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        CGFloat force = 1.0;
        if ([touch respondsToSelector:@selector(force)] && [touch respondsToSelector:@selector(maximumPossibleForce)])
            force = (touch.maximumPossibleForce > 0.0F) ? touch.force / touch.maximumPossibleForce : 0.0F;

        touchpadDevice->handleTouchCancel(reinterpret_cast<uint64_t>(touch),
                                          ouzel::Vector2(static_cast<float>(location.x / self.bounds.size.width),
                                                         static_cast<float>(location.y / self.bounds.size.height)),
                                          static_cast<float>(force));
    }
}

-(void)pressesBegan:(NSSet<UIPress*>*)presses withEvent:(UIPressesEvent*)event
{
    bool forward = false;

    ouzel::input::InputSystemIOS* inputSystemIOS = static_cast<ouzel::input::InputSystemIOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemIOS->getKeyboardDevice();
    for (UIPress* press in presses)
    {
        std::future<bool> f = keyboardDevice->handleKeyPress(ouzel::input::InputSystemIOS::convertKeyCode(press.type));
        if (press.type == UIPressTypeMenu && !f.get())
            forward = true;
    }

    if (forward)
        [super pressesBegan:presses withEvent:event];
}

-(void)pressesEnded:(NSSet<UIPress*>*)presses withEvent:(UIPressesEvent*)event
{
    bool forward = false;

    ouzel::input::InputSystemIOS* inputSystemIOS = static_cast<ouzel::input::InputSystemIOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemIOS->getKeyboardDevice();
    for (UIPress* press in presses)
    {
        std::future<bool> f = keyboardDevice->handleKeyRelease(ouzel::input::InputSystemIOS::convertKeyCode(press.type));
        if (press.type == UIPressTypeMenu && !f.get())
            forward = true;
    }

    if (forward)
        [super pressesEnded:presses withEvent:event];
}

-(void)pressesCancelled:(NSSet<UIPress*>*)presses withEvent:(UIPressesEvent*)event
{
    bool forward = false;

    ouzel::input::InputSystemIOS* inputSystemIOS = static_cast<ouzel::input::InputSystemIOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemIOS->getKeyboardDevice();
    for (UIPress* press in presses)
    {
        std::future<bool> f = keyboardDevice->handleKeyRelease(ouzel::input::InputSystemIOS::convertKeyCode(press.type));
        if (press.type == UIPressTypeMenu && !f.get())
            forward = true;
    }

    if (forward)
        [super pressesCancelled:presses withEvent:event];
}

@end
