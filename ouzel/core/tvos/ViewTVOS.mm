// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <unordered_map>
#import <UIKit/UIPress.h>
#import "ViewTVOS.h"
#include "core/Engine.hpp"
#include "input/tvos/InputSystemTVOS.hpp"

@implementation ViewTVOS

- (void)insertText:(__unused NSString*)text
{
}

- (void)deleteBackward
{
}

- (BOOL)hasText
{
    return NO;
}

- (BOOL)canBecomeFirstResponder
{
    return YES;
}

namespace
{
    ouzel::input::Keyboard::Key convertKeyCode(NSInteger keyCode)
    {
        switch (keyCode)
        {
            case UIPressTypeUpArrow: return ouzel::input::Keyboard::Key::Up;
            case UIPressTypeDownArrow: return ouzel::input::Keyboard::Key::Down;
            case UIPressTypeLeftArrow: return ouzel::input::Keyboard::Key::Left;
            case UIPressTypeRightArrow: return ouzel::input::Keyboard::Key::Right;
            case UIPressTypeSelect: return ouzel::input::Keyboard::Key::Select;
            case UIPressTypeMenu: return ouzel::input::Keyboard::Key::Menu;
            case UIPressTypePlayPause: return ouzel::input::Keyboard::Key::Pause;
            default: return ouzel::input::Keyboard::Key::Unknown;
        }
    }
}

- (void)pressesBegan:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    bool forward = false;

    ouzel::input::InputSystemTVOS* inputSystemTVOS = static_cast<ouzel::input::InputSystemTVOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemTVOS->getKeyboardDevice();
    for (UIPress* press in presses)
    {
        std::future<bool> f = keyboardDevice->handleKeyPress(convertKeyCode(press.type));
        if (press.type == UIPressTypeMenu && !f.get())
            forward = true;
    }

    if (forward)
        [super pressesBegan:presses withEvent:event];
}

- (void)pressesEnded:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    bool forward = false;

    ouzel::input::InputSystemTVOS* inputSystemTVOS = static_cast<ouzel::input::InputSystemTVOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemTVOS->getKeyboardDevice();
    for (UIPress* press in presses)
    {
        std::future<bool> f = keyboardDevice->handleKeyRelease(convertKeyCode(press.type));
        if (press.type == UIPressTypeMenu && !f.get())
            forward = true;
    }

    if (forward)
        [super pressesEnded:presses withEvent:event];
}

- (void)pressesCancelled:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    bool forward = false;

    ouzel::input::InputSystemTVOS* inputSystemTVOS = static_cast<ouzel::input::InputSystemTVOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemTVOS->getKeyboardDevice();
    for (UIPress* press in presses)
    {
        std::future<bool> f = keyboardDevice->handleKeyRelease(convertKeyCode(press.type));
        if (press.type == UIPressTypeMenu && !f.get())
            forward = true;
    }

    if (forward)
        [super pressesCancelled:presses withEvent:event];
}
@end
