// Ouzel by Elviss Strazdins

#include <unordered_map>
#import <UIKit/UIPress.h>
#import "ViewTVOS.h"
#include "../Engine.hpp"
#include "../../input/tvos/InputSystemTVOS.hpp"

@implementation View

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
            case UIPressTypeUpArrow: return ouzel::input::Keyboard::Key::up;
            case UIPressTypeDownArrow: return ouzel::input::Keyboard::Key::down;
            case UIPressTypeLeftArrow: return ouzel::input::Keyboard::Key::left;
            case UIPressTypeRightArrow: return ouzel::input::Keyboard::Key::right;
            case UIPressTypeSelect: return ouzel::input::Keyboard::Key::select;
            case UIPressTypeMenu: return ouzel::input::Keyboard::Key::menu;
            case UIPressTypePlayPause: return ouzel::input::Keyboard::Key::pause;
            default: return ouzel::input::Keyboard::Key::none;
        }
    }
}

- (void)pressesBegan:(NSSet<UIPress*>*)presses withEvent:(__unused UIPressesEvent*)event
{
    bool forward = false;

    auto inputSystemTVOS = static_cast<ouzel::input::tvos::InputSystem*>(ouzel::engine->getInputManager()->getInputSystem());
    auto keyboardDevice = inputSystemTVOS->getKeyboardDevice();
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

    auto inputSystemTVOS = static_cast<ouzel::input::tvos::InputSystem*>(ouzel::engine->getInputManager()->getInputSystem());
    auto keyboardDevice = inputSystemTVOS->getKeyboardDevice();
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

    auto inputSystemTVOS = static_cast<ouzel::input::tvos::InputSystem*>(ouzel::engine->getInputManager()->getInputSystem());
    auto keyboardDevice = inputSystemTVOS->getKeyboardDevice();
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
