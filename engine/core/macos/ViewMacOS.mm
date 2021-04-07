// Ouzel by Elviss Strazdins

#include <unordered_map>
#include <Availability.h>
#import <Carbon/Carbon.h>
#import "ViewMacOS.h"
#include "../Engine.hpp"
#include "../Window.hpp"
#include "../../input/macos/InputSystemMacOS.hpp"
#include "../../utils/Utils.hpp"

@implementation View

- (BOOL)isOpaque
{
    return YES;
}

- (BOOL)mouseDownCanMoveWindow
{
    return YES;
}

- (BOOL)isFlipped
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)acceptsFirstMouse:(__unused NSEvent*)event
{
    return YES;
}

- (void)resetCursorRects
{
    [super resetCursorRects];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto cursor = inputSystemMacOS.getCursor();

    [self addCursorRect:[self bounds] cursor:cursor];
    [cursor set];
}

#if !defined(__MAC_10_12) || __MAC_OS_X_VERSION_MAX_ALLOWED < __MAC_10_12
// kVK_RightCommand is not defined in macOS SDK older than 10.12
enum
{
    kVK_RightCommand = 0x36
};
#endif

namespace
{
    ouzel::input::Keyboard::Key convertKeyCode(std::uint16_t keyCode)
    {
        switch (keyCode)
        {
            case kVK_UpArrow: return ouzel::input::Keyboard::Key::up;
            case kVK_DownArrow: return ouzel::input::Keyboard::Key::down;
            case kVK_LeftArrow: return ouzel::input::Keyboard::Key::left;
            case kVK_RightArrow: return ouzel::input::Keyboard::Key::right;
            case kVK_F1: return ouzel::input::Keyboard::Key::f1;
            case kVK_F2: return ouzel::input::Keyboard::Key::f2;
            case kVK_F3: return ouzel::input::Keyboard::Key::f3;
            case kVK_F4: return ouzel::input::Keyboard::Key::f4;
            case kVK_F5: return ouzel::input::Keyboard::Key::f5;
            case kVK_F6: return ouzel::input::Keyboard::Key::f6;
            case kVK_F7: return ouzel::input::Keyboard::Key::f7;
            case kVK_F8: return ouzel::input::Keyboard::Key::f8;
            case kVK_F9: return ouzel::input::Keyboard::Key::f9;
            case kVK_F10: return ouzel::input::Keyboard::Key::f10;
            case kVK_F11: return ouzel::input::Keyboard::Key::f11;
            case kVK_F12: return ouzel::input::Keyboard::Key::f12;
            case kVK_F13: return ouzel::input::Keyboard::Key::f13;
            case kVK_F14: return ouzel::input::Keyboard::Key::f14;
            case kVK_F15: return ouzel::input::Keyboard::Key::f15;
            case kVK_F16: return ouzel::input::Keyboard::Key::f16;
            case kVK_F17: return ouzel::input::Keyboard::Key::f17;
            case kVK_F18: return ouzel::input::Keyboard::Key::f18;
            case kVK_F19: return ouzel::input::Keyboard::Key::f19;
            case kVK_F20: return ouzel::input::Keyboard::Key::f20;
            case kVK_Home: return ouzel::input::Keyboard::Key::home;
            case kVK_End: return ouzel::input::Keyboard::Key::end;
            case NSInsertFunctionKey: return ouzel::input::Keyboard::Key::insert;
            case kVK_ForwardDelete: return ouzel::input::Keyboard::Key::del;
            case kVK_Help: return ouzel::input::Keyboard::Key::help;
            case NSSelectFunctionKey: return ouzel::input::Keyboard::Key::select;
            case NSPrintFunctionKey: return ouzel::input::Keyboard::Key::print;
            case NSExecuteFunctionKey: return ouzel::input::Keyboard::Key::execute;
            case NSPrintScreenFunctionKey: return ouzel::input::Keyboard::Key::printScreen;
            case NSPauseFunctionKey: return ouzel::input::Keyboard::Key::pause;
            case NSScrollLockFunctionKey: return ouzel::input::Keyboard::Key::scrollLock;
            case kVK_Delete: return ouzel::input::Keyboard::Key::backspace;
            case kVK_Tab: return ouzel::input::Keyboard::Key::tab;
            case kVK_Return: return ouzel::input::Keyboard::Key::enter;
            case kVK_Escape: return ouzel::input::Keyboard::Key::escape;
            case kVK_Control: return ouzel::input::Keyboard::Key::leftControl;
            case kVK_RightControl: return ouzel::input::Keyboard::Key::rightControl;
            case kVK_Command: return ouzel::input::Keyboard::Key::leftSuper;
            case kVK_RightCommand: return ouzel::input::Keyboard::Key::rightSuper;
            case kVK_Shift: return ouzel::input::Keyboard::Key::leftShift;
            case kVK_RightShift: return ouzel::input::Keyboard::Key::rightShift;
            case kVK_Option: return ouzel::input::Keyboard::Key::leftAlt;
            case kVK_RightOption: return ouzel::input::Keyboard::Key::rightAlt;
            case kVK_Space: return ouzel::input::Keyboard::Key::space;

            case kVK_ANSI_A: return ouzel::input::Keyboard::Key::a;
            case kVK_ANSI_B: return ouzel::input::Keyboard::Key::b;
            case kVK_ANSI_C: return ouzel::input::Keyboard::Key::c;
            case kVK_ANSI_D: return ouzel::input::Keyboard::Key::d;
            case kVK_ANSI_E: return ouzel::input::Keyboard::Key::e;
            case kVK_ANSI_F: return ouzel::input::Keyboard::Key::f;
            case kVK_ANSI_G: return ouzel::input::Keyboard::Key::g;
            case kVK_ANSI_H: return ouzel::input::Keyboard::Key::h;
            case kVK_ANSI_I: return ouzel::input::Keyboard::Key::i;
            case kVK_ANSI_J: return ouzel::input::Keyboard::Key::j;
            case kVK_ANSI_K: return ouzel::input::Keyboard::Key::k;
            case kVK_ANSI_L: return ouzel::input::Keyboard::Key::l;
            case kVK_ANSI_M: return ouzel::input::Keyboard::Key::m;
            case kVK_ANSI_N: return ouzel::input::Keyboard::Key::n;
            case kVK_ANSI_O: return ouzel::input::Keyboard::Key::o;
            case kVK_ANSI_P: return ouzel::input::Keyboard::Key::p;
            case kVK_ANSI_Q: return ouzel::input::Keyboard::Key::q;
            case kVK_ANSI_R: return ouzel::input::Keyboard::Key::r;
            case kVK_ANSI_S: return ouzel::input::Keyboard::Key::s;
            case kVK_ANSI_T: return ouzel::input::Keyboard::Key::t;
            case kVK_ANSI_U: return ouzel::input::Keyboard::Key::u;
            case kVK_ANSI_V: return ouzel::input::Keyboard::Key::v;
            case kVK_ANSI_W: return ouzel::input::Keyboard::Key::w;
            case kVK_ANSI_X: return ouzel::input::Keyboard::Key::x;
            case kVK_ANSI_Y: return ouzel::input::Keyboard::Key::y;
            case kVK_ANSI_Z: return ouzel::input::Keyboard::Key::z;

            case kVK_ANSI_0: return ouzel::input::Keyboard::Key::num0;
            case kVK_ANSI_1: return ouzel::input::Keyboard::Key::num1;
            case kVK_ANSI_2: return ouzel::input::Keyboard::Key::num2;
            case kVK_ANSI_3: return ouzel::input::Keyboard::Key::num3;
            case kVK_ANSI_4: return ouzel::input::Keyboard::Key::num4;
            case kVK_ANSI_5: return ouzel::input::Keyboard::Key::num5;
            case kVK_ANSI_6: return ouzel::input::Keyboard::Key::num6;
            case kVK_ANSI_7: return ouzel::input::Keyboard::Key::num7;
            case kVK_ANSI_8: return ouzel::input::Keyboard::Key::num8;
            case kVK_ANSI_9: return ouzel::input::Keyboard::Key::num9;

            case kVK_ANSI_Comma: return ouzel::input::Keyboard::Key::comma;
            case kVK_ANSI_Period: return ouzel::input::Keyboard::Key::period;
            case kVK_PageUp: return ouzel::input::Keyboard::Key::pageUp;
            case kVK_PageDown: return ouzel::input::Keyboard::Key::pageDown;

            case kVK_ANSI_Keypad0: return ouzel::input::Keyboard::Key::numpad0;
            case kVK_ANSI_Keypad1: return ouzel::input::Keyboard::Key::numpad1;
            case kVK_ANSI_Keypad2: return ouzel::input::Keyboard::Key::numpad2;
            case kVK_ANSI_Keypad3: return ouzel::input::Keyboard::Key::numpad3;
            case kVK_ANSI_Keypad4: return ouzel::input::Keyboard::Key::numpad4;
            case kVK_ANSI_Keypad5: return ouzel::input::Keyboard::Key::numpad5;
            case kVK_ANSI_Keypad6: return ouzel::input::Keyboard::Key::numpad6;
            case kVK_ANSI_Keypad7: return ouzel::input::Keyboard::Key::numpad7;
            case kVK_ANSI_Keypad8: return ouzel::input::Keyboard::Key::numpad8;
            case kVK_ANSI_Keypad9: return ouzel::input::Keyboard::Key::numpad9;

            case kVK_ANSI_KeypadDecimal: return ouzel::input::Keyboard::Key::numpadDecimal;
            case kVK_ANSI_KeypadMultiply: return ouzel::input::Keyboard::Key::numpadMultiply;
            case kVK_ANSI_KeypadPlus: return ouzel::input::Keyboard::Key::numpadPlus;
            case kVK_ANSI_KeypadClear: return ouzel::input::Keyboard::Key::numLock;
            case kVK_ANSI_KeypadDivide: return ouzel::input::Keyboard::Key::numpadDivide;
            case kVK_ANSI_KeypadEnter: return ouzel::input::Keyboard::Key::numpadEnter;
            case kVK_ANSI_KeypadMinus: return ouzel::input::Keyboard::Key::numpadMinus;

            case kVK_ANSI_Semicolon: return ouzel::input::Keyboard::Key::semicolon;
            case kVK_ANSI_Slash: return ouzel::input::Keyboard::Key::slash;
            case kVK_ANSI_Grave: return ouzel::input::Keyboard::Key::grave;
            case kVK_ANSI_LeftBracket: return ouzel::input::Keyboard::Key::leftBracket;
            case kVK_ANSI_Backslash: return ouzel::input::Keyboard::Key::backslash;
            case kVK_ANSI_RightBracket: return ouzel::input::Keyboard::Key::rightBracket;
            case 0x6E: return ouzel::input::Keyboard::Key::menu;
            case 0x7F: return ouzel::input::Keyboard::Key::power;

            case kVK_JIS_Yen: return ouzel::input::Keyboard::Key::yen;
            case kVK_JIS_KeypadComma: return ouzel::input::Keyboard::Key::numpadJpcomma;
            case kVK_JIS_Kana: return ouzel::input::Keyboard::Key::kana;

            case kVK_VolumeUp: return ouzel::input::Keyboard::Key::volumeUp;
            case kVK_VolumeDown: return ouzel::input::Keyboard::Key::volumeDown;
            case kVK_Mute: return ouzel::input::Keyboard::Key::mute;
            default: return ouzel::input::Keyboard::Key::none;
        }
    }
}

- (void)keyDown:(NSEvent*)event
{
    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto keyboardDevice = inputSystemMacOS.getKeyboardDevice();
    keyboardDevice->handleKeyPress(convertKeyCode(event.keyCode));
}

- (void)keyUp:(NSEvent*)event
{
    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto keyboardDevice = inputSystemMacOS.getKeyboardDevice();
    keyboardDevice->handleKeyRelease(convertKeyCode(event.keyCode));
}

- (void)flagsChanged:(NSEvent*)event
{
    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto keyboardDevice = inputSystemMacOS.getKeyboardDevice();

    NSUInteger mask = 0;

    switch (event.keyCode)
    {
        case kVK_Control: mask = NX_DEVICELCTLKEYMASK; break;
        case kVK_RightControl: mask = NX_DEVICERCTLKEYMASK; break;
        case kVK_Command: mask = NX_DEVICELCMDKEYMASK; break;
        case kVK_RightCommand: mask = NX_DEVICERCMDKEYMASK; break;
        case kVK_Shift: mask = NX_DEVICELSHIFTKEYMASK; break;
        case kVK_RightShift: mask = NX_DEVICERSHIFTKEYMASK; break;
        case kVK_Option: mask = NX_DEVICELALTKEYMASK; break;
        case kVK_RightOption: mask = NX_DEVICERALTKEYMASK; break;
    }

    if (mask)
    {
        if (event.modifierFlags & mask)
            keyboardDevice->handleKeyPress(convertKeyCode(event.keyCode));
        else
            keyboardDevice->handleKeyRelease(convertKeyCode(event.keyCode));
    }
}

- (void)doCommandBySelector:(__unused SEL)selector
{
    // implement this method to not beep on Command-Escape
}

- (void)mouseDown:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto mouseDevice = inputSystemMacOS.getMouseDevice();
    const ouzel::Vector<float, 2> position{
        static_cast<float>(location.x),
        static_cast<float>(location.y)
    };

    mouseDevice->handleButtonPress(ouzel::input::Mouse::Button::left,
                                   ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
}

- (void)mouseUp:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto mouseDevice = inputSystemMacOS.getMouseDevice();
    const ouzel::Vector<float, 2> position{
        static_cast<float>(location.x),
        static_cast<float>(location.y)
    };

    mouseDevice->handleButtonRelease(ouzel::input::Mouse::Button::left,
                                     ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
}

- (void)rightMouseDown:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto mouseDevice = inputSystemMacOS.getMouseDevice();
    const ouzel::Vector<float, 2> position{
        static_cast<float>(location.x),
        static_cast<float>(location.y)
    };

    mouseDevice->handleButtonPress(ouzel::input::Mouse::Button::right,
                                   ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
}

- (void)rightMouseUp:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto mouseDevice = inputSystemMacOS.getMouseDevice();
    const ouzel::Vector<float, 2> position{
        static_cast<float>(location.x),
        static_cast<float>(location.y)
    };

    mouseDevice->handleButtonRelease(ouzel::input::Mouse::Button::right,
                                     ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
}

- (void)otherMouseDown:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto mouseDevice = inputSystemMacOS.getMouseDevice();
    const ouzel::Vector<float, 2> position{
        static_cast<float>(location.x),
        static_cast<float>(location.y)
    };

    mouseDevice->handleButtonPress(ouzel::input::Mouse::Button::middle,
                                   ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
}

- (void)otherMouseUp:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto mouseDevice = inputSystemMacOS.getMouseDevice();
    const ouzel::Vector<float, 2> position{
        static_cast<float>(location.x),
        static_cast<float>(location.y)
    };

    mouseDevice->handleButtonRelease(ouzel::input::Mouse::Button::middle,
                                     ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
}

- (void)mouseMoved:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto mouseDevice = inputSystemMacOS.getMouseDevice();
    const ouzel::Vector<float, 2> position{
        static_cast<float>(location.x),
        static_cast<float>(location.y)
    };

    mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
}

- (void)mouseDragged:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto mouseDevice = inputSystemMacOS.getMouseDevice();
    const ouzel::Vector<float, 2> position{
        static_cast<float>(location.x),
        static_cast<float>(location.y)
    };

    mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
}

- (void)rightMouseDragged:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto mouseDevice = inputSystemMacOS.getMouseDevice();
    const ouzel::Vector<float, 2> position{
        static_cast<float>(location.x),
        static_cast<float>(location.y)
    };

    mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
}

- (void)otherMouseDragged:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto mouseDevice = inputSystemMacOS.getMouseDevice();
    const ouzel::Vector<float, 2> position{
        static_cast<float>(location.x),
        static_cast<float>(location.y)
    };

    mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
}

- (void)scrollWheel:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto mouseDevice = inputSystemMacOS.getMouseDevice();
    const ouzel::Vector<float, 2> position{
        static_cast<float>(location.x),
        static_cast<float>(location.y)
    };

    mouseDevice->handleScroll(ouzel::Vector<float, 2>{static_cast<float>(event.scrollingDeltaX), static_cast<float>(event.scrollingDeltaY)},
                              ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
}

- (void)touchesBeganWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseBegan inView:self];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto touchpadDevice = inputSystemMacOS.getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        const CGPoint location = touch.normalizedPosition;
        const ouzel::Vector<float, 2> position{
            static_cast<float>(location.x),
            static_cast<float>(location.y)
        };

        touchpadDevice->handleTouchBegin(ouzel::bitCast<std::uintptr_t>(touch), position, 1.0F);
    }
}

- (void)touchesMovedWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseMoved inView:self];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto touchpadDevice = inputSystemMacOS.getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        const CGPoint location = touch.normalizedPosition;
        const ouzel::Vector<float, 2> position{
            static_cast<float>(location.x),
            static_cast<float>(location.y)
        };

        touchpadDevice->handleTouchMove(ouzel::bitCast<std::uintptr_t>(touch), position, 1.0F);
    }
}

- (void)touchesEndedWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseEnded inView:self];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto touchpadDevice = inputSystemMacOS.getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        const CGPoint location = touch.normalizedPosition;
        const ouzel::Vector<float, 2> position{
            static_cast<float>(location.x),
            static_cast<float>(location.y)
        };

        touchpadDevice->handleTouchEnd(ouzel::bitCast<std::uintptr_t>(touch), position, 1.0F);
    }
}

- (void)touchesCancelledWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseCancelled inView:self];

    auto& inputSystemMacOS = ouzel::engine->getInputManager()->getInputSystem();
    const auto touchpadDevice = inputSystemMacOS.getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        const CGPoint location = touch.normalizedPosition;
        const ouzel::Vector<float, 2> position{
            static_cast<float>(location.x),
            static_cast<float>(location.y)
        };

        touchpadDevice->handleTouchCancel(ouzel::bitCast<std::uintptr_t>(touch), position, 1.0F);
    }
}

- (void)swipeWithEvent:(__unused NSEvent*)event
{
}

- (void)rotateWithEvent:(__unused NSEvent*)event
{
}

- (void)magnifyWithEvent:(__unused NSEvent*)event
{
}
@end
