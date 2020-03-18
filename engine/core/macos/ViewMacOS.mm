// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <unordered_map>
#include <Availability.h>
#import <Carbon/Carbon.h>
#import "ViewMacOS.h"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "input/macos/InputSystemMacOS.hpp"

@implementation ViewMacOS

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

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    NSCursor* cursor = inputSystemMacOS->getCursor();

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
            case kVK_UpArrow: return ouzel::input::Keyboard::Key::Up;
            case kVK_DownArrow: return ouzel::input::Keyboard::Key::Down;
            case kVK_LeftArrow: return ouzel::input::Keyboard::Key::Left;
            case kVK_RightArrow: return ouzel::input::Keyboard::Key::Right;
            case kVK_F1: return ouzel::input::Keyboard::Key::F1;
            case kVK_F2: return ouzel::input::Keyboard::Key::F2;
            case kVK_F3: return ouzel::input::Keyboard::Key::F3;
            case kVK_F4: return ouzel::input::Keyboard::Key::F4;
            case kVK_F5: return ouzel::input::Keyboard::Key::F5;
            case kVK_F6: return ouzel::input::Keyboard::Key::F6;
            case kVK_F7: return ouzel::input::Keyboard::Key::F7;
            case kVK_F8: return ouzel::input::Keyboard::Key::F8;
            case kVK_F9: return ouzel::input::Keyboard::Key::F9;
            case kVK_F10: return ouzel::input::Keyboard::Key::F10;
            case kVK_F11: return ouzel::input::Keyboard::Key::F11;
            case kVK_F12: return ouzel::input::Keyboard::Key::F12;
            case kVK_F13: return ouzel::input::Keyboard::Key::F13;
            case kVK_F14: return ouzel::input::Keyboard::Key::F14;
            case kVK_F15: return ouzel::input::Keyboard::Key::F15;
            case kVK_F16: return ouzel::input::Keyboard::Key::F16;
            case kVK_F17: return ouzel::input::Keyboard::Key::F17;
            case kVK_F18: return ouzel::input::Keyboard::Key::F18;
            case kVK_F19: return ouzel::input::Keyboard::Key::F19;
            case kVK_F20: return ouzel::input::Keyboard::Key::F20;
            case kVK_Home: return ouzel::input::Keyboard::Key::Home;
            case kVK_End: return ouzel::input::Keyboard::Key::End;
            case NSInsertFunctionKey: return ouzel::input::Keyboard::Key::Insert;
            case kVK_ForwardDelete: return ouzel::input::Keyboard::Key::Del;
            case kVK_Help: return ouzel::input::Keyboard::Key::Help;
            case NSSelectFunctionKey: return ouzel::input::Keyboard::Key::Select;
            case NSPrintFunctionKey: return ouzel::input::Keyboard::Key::Print;
            case NSExecuteFunctionKey: return ouzel::input::Keyboard::Key::Execute;
            case NSPrintScreenFunctionKey: return ouzel::input::Keyboard::Key::PrintScreen;
            case NSPauseFunctionKey: return ouzel::input::Keyboard::Key::Pause;
            case NSScrollLockFunctionKey: return ouzel::input::Keyboard::Key::ScrollLock;
            case kVK_Delete: return ouzel::input::Keyboard::Key::Backspace;
            case kVK_Tab: return ouzel::input::Keyboard::Key::Tab;
            case kVK_Return: return ouzel::input::Keyboard::Key::Enter;
            case kVK_Escape: return ouzel::input::Keyboard::Key::Escape;
            case kVK_Control: return ouzel::input::Keyboard::Key::LeftControl;
            case kVK_RightControl: return ouzel::input::Keyboard::Key::RightControl;
            case kVK_Command: return ouzel::input::Keyboard::Key::LeftSuper;
            case kVK_RightCommand: return ouzel::input::Keyboard::Key::RightSuper;
            case kVK_Shift: return ouzel::input::Keyboard::Key::LeftShift;
            case kVK_RightShift: return ouzel::input::Keyboard::Key::RightShift;
            case kVK_Option: return ouzel::input::Keyboard::Key::LeftAlt;
            case kVK_RightOption: return ouzel::input::Keyboard::Key::RightAlt;
            case kVK_Space: return ouzel::input::Keyboard::Key::Space;

            case kVK_ANSI_A: return ouzel::input::Keyboard::Key::A;
            case kVK_ANSI_B: return ouzel::input::Keyboard::Key::B;
            case kVK_ANSI_C: return ouzel::input::Keyboard::Key::C;
            case kVK_ANSI_D: return ouzel::input::Keyboard::Key::D;
            case kVK_ANSI_E: return ouzel::input::Keyboard::Key::E;
            case kVK_ANSI_F: return ouzel::input::Keyboard::Key::F;
            case kVK_ANSI_G: return ouzel::input::Keyboard::Key::G;
            case kVK_ANSI_H: return ouzel::input::Keyboard::Key::H;
            case kVK_ANSI_I: return ouzel::input::Keyboard::Key::I;
            case kVK_ANSI_J: return ouzel::input::Keyboard::Key::J;
            case kVK_ANSI_K: return ouzel::input::Keyboard::Key::K;
            case kVK_ANSI_L: return ouzel::input::Keyboard::Key::L;
            case kVK_ANSI_M: return ouzel::input::Keyboard::Key::M;
            case kVK_ANSI_N: return ouzel::input::Keyboard::Key::N;
            case kVK_ANSI_O: return ouzel::input::Keyboard::Key::O;
            case kVK_ANSI_P: return ouzel::input::Keyboard::Key::P;
            case kVK_ANSI_Q: return ouzel::input::Keyboard::Key::Q;
            case kVK_ANSI_R: return ouzel::input::Keyboard::Key::R;
            case kVK_ANSI_S: return ouzel::input::Keyboard::Key::S;
            case kVK_ANSI_T: return ouzel::input::Keyboard::Key::T;
            case kVK_ANSI_U: return ouzel::input::Keyboard::Key::U;
            case kVK_ANSI_V: return ouzel::input::Keyboard::Key::V;
            case kVK_ANSI_W: return ouzel::input::Keyboard::Key::W;
            case kVK_ANSI_X: return ouzel::input::Keyboard::Key::X;
            case kVK_ANSI_Y: return ouzel::input::Keyboard::Key::Y;
            case kVK_ANSI_Z: return ouzel::input::Keyboard::Key::Z;

            case kVK_ANSI_0: return ouzel::input::Keyboard::Key::Num0;
            case kVK_ANSI_1: return ouzel::input::Keyboard::Key::Num1;
            case kVK_ANSI_2: return ouzel::input::Keyboard::Key::Num2;
            case kVK_ANSI_3: return ouzel::input::Keyboard::Key::Num3;
            case kVK_ANSI_4: return ouzel::input::Keyboard::Key::Num4;
            case kVK_ANSI_5: return ouzel::input::Keyboard::Key::Num5;
            case kVK_ANSI_6: return ouzel::input::Keyboard::Key::Num6;
            case kVK_ANSI_7: return ouzel::input::Keyboard::Key::Num7;
            case kVK_ANSI_8: return ouzel::input::Keyboard::Key::Num8;
            case kVK_ANSI_9: return ouzel::input::Keyboard::Key::Num9;

            case kVK_ANSI_Comma: return ouzel::input::Keyboard::Key::Comma;
            case kVK_ANSI_Period: return ouzel::input::Keyboard::Key::Period;
            case kVK_PageUp: return ouzel::input::Keyboard::Key::PageUp;
            case kVK_PageDown: return ouzel::input::Keyboard::Key::PageDown;

            case kVK_ANSI_Keypad0: return ouzel::input::Keyboard::Key::Numpad0;
            case kVK_ANSI_Keypad1: return ouzel::input::Keyboard::Key::Numpad1;
            case kVK_ANSI_Keypad2: return ouzel::input::Keyboard::Key::Numpad2;
            case kVK_ANSI_Keypad3: return ouzel::input::Keyboard::Key::Numpad3;
            case kVK_ANSI_Keypad4: return ouzel::input::Keyboard::Key::Numpad4;
            case kVK_ANSI_Keypad5: return ouzel::input::Keyboard::Key::Numpad5;
            case kVK_ANSI_Keypad6: return ouzel::input::Keyboard::Key::Numpad6;
            case kVK_ANSI_Keypad7: return ouzel::input::Keyboard::Key::Numpad7;
            case kVK_ANSI_Keypad8: return ouzel::input::Keyboard::Key::Numpad8;
            case kVK_ANSI_Keypad9: return ouzel::input::Keyboard::Key::Numpad9;

            case kVK_ANSI_KeypadDecimal: return ouzel::input::Keyboard::Key::NumpadDecimal;
            case kVK_ANSI_KeypadMultiply: return ouzel::input::Keyboard::Key::NumpadMultiply;
            case kVK_ANSI_KeypadPlus: return ouzel::input::Keyboard::Key::NumpadPlus;
            case kVK_ANSI_KeypadClear: return ouzel::input::Keyboard::Key::NumLock;
            case kVK_ANSI_KeypadDivide: return ouzel::input::Keyboard::Key::NumpadDivide;
            case kVK_ANSI_KeypadEnter: return ouzel::input::Keyboard::Key::NumpadEnter;
            case kVK_ANSI_KeypadMinus: return ouzel::input::Keyboard::Key::NumpadMinus;

            case kVK_ANSI_Semicolon: return ouzel::input::Keyboard::Key::Semicolon;
            case kVK_ANSI_Slash: return ouzel::input::Keyboard::Key::Slash;
            case kVK_ANSI_Grave: return ouzel::input::Keyboard::Key::Grave;
            case kVK_ANSI_LeftBracket: return ouzel::input::Keyboard::Key::LeftBracket;
            case kVK_ANSI_Backslash: return ouzel::input::Keyboard::Key::Backslash;
            case kVK_ANSI_RightBracket: return ouzel::input::Keyboard::Key::RightBracket;
            case 0x6E: return ouzel::input::Keyboard::Key::Menu;
            case 0x7F: return ouzel::input::Keyboard::Key::Power;

            case kVK_JIS_Yen: return ouzel::input::Keyboard::Key::Yen;
            case kVK_JIS_KeypadComma: return ouzel::input::Keyboard::Key::NumpadJpcomma;
            case kVK_JIS_Kana: return ouzel::input::Keyboard::Key::Kana;

            case kVK_VolumeUp: return ouzel::input::Keyboard::Key::VolumeUp;
            case kVK_VolumeDown: return ouzel::input::Keyboard::Key::VolumeDown;
            case kVK_Mute: return ouzel::input::Keyboard::Key::Mute;
            default: return ouzel::input::Keyboard::Key::Unknown;
        }
    }
}

- (void)keyDown:(NSEvent*)event
{
    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemMacOS->getKeyboardDevice();
    keyboardDevice->handleKeyPress(convertKeyCode(event.keyCode));
}

- (void)keyUp:(NSEvent*)event
{
    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemMacOS->getKeyboardDevice();
    keyboardDevice->handleKeyRelease(convertKeyCode(event.keyCode));
}

- (void)flagsChanged:(NSEvent*)event
{
    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemMacOS->getKeyboardDevice();

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

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleButtonPress(ouzel::input::Mouse::Button::Left,
                                   ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2F(static_cast<float>(location.x),
                                                                                                                 static_cast<float>(location.y))));
}

- (void)mouseUp:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleButtonRelease(ouzel::input::Mouse::Button::Left,
                                     ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2F(static_cast<float>(location.x),
                                                                                                                   static_cast<float>(location.y))));
}

- (void)rightMouseDown:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleButtonPress(ouzel::input::Mouse::Button::Right,
                                   ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2F(static_cast<float>(location.x),
                                                                                                                 static_cast<float>(location.y))));
}

- (void)rightMouseUp:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleButtonRelease(ouzel::input::Mouse::Button::Right,
                                     ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2F(static_cast<float>(location.x),
                                                                                                                   static_cast<float>(location.y))));
}

- (void)otherMouseDown:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleButtonPress(ouzel::input::Mouse::Button::Middle,
                                   ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2F(static_cast<float>(location.x),
                                                                                                                 static_cast<float>(location.y))));
}

- (void)otherMouseUp:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleButtonRelease(ouzel::input::Mouse::Button::Middle,
                                     ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2F(static_cast<float>(location.x),
                                                                                                                   static_cast<float>(location.y))));
}

- (void)mouseMoved:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2F(static_cast<float>(location.x),
                                                                                                          static_cast<float>(location.y))));
}

- (void)mouseDragged:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2F(static_cast<float>(location.x),
                                                                                                          static_cast<float>(location.y))));
}

- (void)rightMouseDragged:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2F(static_cast<float>(location.x),
                                                                                                          static_cast<float>(location.y))));
}

- (void)otherMouseDragged:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2F(static_cast<float>(location.x),
                                                                                                          static_cast<float>(location.y))));
}

- (void)scrollWheel:(NSEvent*)event
{
    const NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleScroll(ouzel::Vector2F(static_cast<float>(event.scrollingDeltaX),
                                              static_cast<float>(event.scrollingDeltaY)),
                              ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2F(static_cast<float>(location.x),
                                                                                                            static_cast<float>(location.y))));
}

- (void)touchesBeganWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseBegan inView:self];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDevice* touchpadDevice = inputSystemMacOS->getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        const CGPoint location = touch.normalizedPosition;

        std::uint64_t touchId;
        memcpy(&touchId, &touch, sizeof(touch));

        touchpadDevice->handleTouchBegin(touchId,
                                         ouzel::Vector2F(static_cast<float>(location.x),
                                                         static_cast<float>(location.y)),
                                         1.0F);
    }
}

- (void)touchesMovedWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseMoved inView:self];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDevice* touchpadDevice = inputSystemMacOS->getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        const CGPoint location = touch.normalizedPosition;

        std::uint64_t touchId;
        memcpy(&touchId, &touch, sizeof(touch));

        touchpadDevice->handleTouchMove(touchId,
                                        ouzel::Vector2F(static_cast<float>(location.x),
                                                        static_cast<float>(location.y)),
                                        1.0F);
    }
}

- (void)touchesEndedWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseEnded inView:self];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDevice* touchpadDevice = inputSystemMacOS->getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        const CGPoint location = touch.normalizedPosition;

        std::uint64_t touchId;
        memcpy(&touchId, &touch, sizeof(touch));

        touchpadDevice->handleTouchEnd(touchId,
                                       ouzel::Vector2F(static_cast<float>(location.x),
                                                       static_cast<float>(location.y)),
                                       1.0F);
    }
}

- (void)touchesCancelledWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseCancelled inView:self];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDevice* touchpadDevice = inputSystemMacOS->getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        const CGPoint location = touch.normalizedPosition;

        std::uint64_t touchId;
        memcpy(&touchId, &touch, sizeof(touch));

        touchpadDevice->handleTouchCancel(touchId,
                                          ouzel::Vector2F(static_cast<float>(location.x),
                                                          static_cast<float>(location.y)),
                                          1.0F);
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
