// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <unordered_map>
#import <UIKit/UIPress.h>
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

static const std::unordered_map<NSInteger, ouzel::input::Keyboard::Key> keyMap = {
    {UIPressTypeUpArrow, ouzel::input::Keyboard::Key::UP},
    {UIPressTypeDownArrow, ouzel::input::Keyboard::Key::DOWN},
    {UIPressTypeLeftArrow, ouzel::input::Keyboard::Key::LEFT},
    {UIPressTypeRightArrow, ouzel::input::Keyboard::Key::RIGHT},
    {UIPressTypeSelect, ouzel::input::Keyboard::Key::SELECT},
    {UIPressTypeMenu, ouzel::input::Keyboard::Key::MENU},
    {UIPressTypePlayPause, ouzel::input::Keyboard::Key::PAUSE}
};

static ouzel::input::Keyboard::Key convertKeyCode(NSInteger keyCode)
{
    auto i = keyMap.find(keyCode);

    if (i != keyMap.end())
        return i->second;
    else
        return ouzel::input::Keyboard::Key::NONE;
}

-(void)pressesBegan:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
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

-(void)pressesEnded:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
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

-(void)pressesCancelled:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
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
