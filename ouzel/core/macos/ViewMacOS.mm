// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#import <unordered_map>
#import <Carbon/Carbon.h>
#import "ViewMacOS.h"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "input/macos/InputSystemMacOS.hpp"

@implementation ViewMacOS

-(BOOL)isOpaque
{
    return YES;
}

-(BOOL)mouseDownCanMoveWindow
{
    return YES;
}

-(BOOL)isFlipped
{
    return YES;
}

-(BOOL)acceptsFirstResponder
{
    return YES;
}

-(BOOL)acceptsFirstMouse:(__unused NSEvent*)event
{
    return YES;
}

-(void)resetCursorRects
{
    [super resetCursorRects];

    /*ouzel::input::InputManagerMacOS* inputMacOS = static_cast<ouzel::input::InputManagerMacOS*>(ouzel::engine->getInputManager());

    if (inputMacOS->isCursorVisible())
    {
        [self addCursorRect:[self bounds] cursor:inputMacOS->getNativeCursor()];
        [inputMacOS->getNativeCursor() set];
    }
    else
    {
        [self addCursorRect:[self bounds] cursor:inputMacOS->getEmptyCursor()];
        [inputMacOS->getEmptyCursor() set];
    }*/
}

#if !defined(__MAC_10_12) || __MAC_OS_X_VERSION_MAX_ALLOWED < __MAC_10_12
enum
{
    kVK_RightCommand = 0x36
};
#endif

static const std::unordered_map<uint16_t, ouzel::input::Keyboard::Key> keyMap = {
    {kVK_UpArrow, ouzel::input::Keyboard::Key::UP},
    {kVK_DownArrow, ouzel::input::Keyboard::Key::DOWN},
    {kVK_LeftArrow, ouzel::input::Keyboard::Key::LEFT},
    {kVK_RightArrow, ouzel::input::Keyboard::Key::RIGHT},
    {kVK_F1, ouzel::input::Keyboard::Key::F1},
    {kVK_F2, ouzel::input::Keyboard::Key::F2},
    {kVK_F3, ouzel::input::Keyboard::Key::F3},
    {kVK_F4, ouzel::input::Keyboard::Key::F4},
    {kVK_F5, ouzel::input::Keyboard::Key::F5},
    {kVK_F6, ouzel::input::Keyboard::Key::F6},
    {kVK_F7, ouzel::input::Keyboard::Key::F7},
    {kVK_F8, ouzel::input::Keyboard::Key::F8},
    {kVK_F9, ouzel::input::Keyboard::Key::F9},
    {kVK_F10, ouzel::input::Keyboard::Key::F10},
    {kVK_F11, ouzel::input::Keyboard::Key::F11},
    {kVK_F12, ouzel::input::Keyboard::Key::F12},
    {kVK_F13, ouzel::input::Keyboard::Key::F13},
    {kVK_F14, ouzel::input::Keyboard::Key::F14},
    {kVK_F15, ouzel::input::Keyboard::Key::F15},
    {kVK_F16, ouzel::input::Keyboard::Key::F16},
    {kVK_F17, ouzel::input::Keyboard::Key::F17},
    {kVK_F18, ouzel::input::Keyboard::Key::F18},
    {kVK_F19, ouzel::input::Keyboard::Key::F19},
    {kVK_F20, ouzel::input::Keyboard::Key::F20},
    {kVK_Home, ouzel::input::Keyboard::Key::HOME},
    {kVK_End, ouzel::input::Keyboard::Key::END},
    {NSInsertFunctionKey, ouzel::input::Keyboard::Key::INSERT},
    {kVK_ForwardDelete, ouzel::input::Keyboard::Key::DEL},
    {kVK_Help, ouzel::input::Keyboard::Key::HELP},
    {NSSelectFunctionKey, ouzel::input::Keyboard::Key::SELECT},
    {NSPrintFunctionKey, ouzel::input::Keyboard::Key::PRINT},
    {NSExecuteFunctionKey, ouzel::input::Keyboard::Key::EXECUTE},
    {NSPrintScreenFunctionKey, ouzel::input::Keyboard::Key::PRINT_SCREEN},
    {NSPauseFunctionKey, ouzel::input::Keyboard::Key::PAUSE},
    {NSScrollLockFunctionKey, ouzel::input::Keyboard::Key::SCROLL_LOCK},
    {kVK_Delete, ouzel::input::Keyboard::Key::BACKSPACE},
    {kVK_Tab, ouzel::input::Keyboard::Key::TAB},
    {kVK_Return, ouzel::input::Keyboard::Key::ENTER},
    {kVK_Escape, ouzel::input::Keyboard::Key::ESCAPE},
    {kVK_Control, ouzel::input::Keyboard::Key::LEFT_CONTROL},
    {kVK_RightControl, ouzel::input::Keyboard::Key::RIGHT_CONTROL},
    {kVK_Command, ouzel::input::Keyboard::Key::LEFT_SUPER},
    {kVK_RightCommand, ouzel::input::Keyboard::Key::RIGHT_SUPER},
    {kVK_Shift, ouzel::input::Keyboard::Key::LEFT_SHIFT},
    {kVK_RightShift, ouzel::input::Keyboard::Key::RIGHT_SHIFT},
    {kVK_Option, ouzel::input::Keyboard::Key::LEFT_ALT},
    {kVK_RightOption, ouzel::input::Keyboard::Key::RIGHT_ALT},
    {kVK_Space, ouzel::input::Keyboard::Key::SPACE},

    {kVK_ANSI_A, ouzel::input::Keyboard::Key::A},
    {kVK_ANSI_B, ouzel::input::Keyboard::Key::B},
    {kVK_ANSI_C, ouzel::input::Keyboard::Key::C},
    {kVK_ANSI_D, ouzel::input::Keyboard::Key::D},
    {kVK_ANSI_E, ouzel::input::Keyboard::Key::E},
    {kVK_ANSI_F, ouzel::input::Keyboard::Key::F},
    {kVK_ANSI_G, ouzel::input::Keyboard::Key::G},
    {kVK_ANSI_H, ouzel::input::Keyboard::Key::H},
    {kVK_ANSI_I, ouzel::input::Keyboard::Key::I},
    {kVK_ANSI_J, ouzel::input::Keyboard::Key::J},
    {kVK_ANSI_K, ouzel::input::Keyboard::Key::K},
    {kVK_ANSI_L, ouzel::input::Keyboard::Key::L},
    {kVK_ANSI_M, ouzel::input::Keyboard::Key::M},
    {kVK_ANSI_N, ouzel::input::Keyboard::Key::N},
    {kVK_ANSI_O, ouzel::input::Keyboard::Key::O},
    {kVK_ANSI_P, ouzel::input::Keyboard::Key::P},
    {kVK_ANSI_Q, ouzel::input::Keyboard::Key::Q},
    {kVK_ANSI_R, ouzel::input::Keyboard::Key::R},
    {kVK_ANSI_S, ouzel::input::Keyboard::Key::S},
    {kVK_ANSI_T, ouzel::input::Keyboard::Key::T},
    {kVK_ANSI_U, ouzel::input::Keyboard::Key::U},
    {kVK_ANSI_V, ouzel::input::Keyboard::Key::V},
    {kVK_ANSI_W, ouzel::input::Keyboard::Key::W},
    {kVK_ANSI_X, ouzel::input::Keyboard::Key::X},
    {kVK_ANSI_Y, ouzel::input::Keyboard::Key::Y},
    {kVK_ANSI_Z, ouzel::input::Keyboard::Key::Z},

    {kVK_ANSI_0, ouzel::input::Keyboard::Key::NUM_0},
    {kVK_ANSI_1, ouzel::input::Keyboard::Key::NUM_1},
    {kVK_ANSI_2, ouzel::input::Keyboard::Key::NUM_2},
    {kVK_ANSI_3, ouzel::input::Keyboard::Key::NUM_3},
    {kVK_ANSI_4, ouzel::input::Keyboard::Key::NUM_4},
    {kVK_ANSI_5, ouzel::input::Keyboard::Key::NUM_5},
    {kVK_ANSI_6, ouzel::input::Keyboard::Key::NUM_6},
    {kVK_ANSI_7, ouzel::input::Keyboard::Key::NUM_7},
    {kVK_ANSI_8, ouzel::input::Keyboard::Key::NUM_8},
    {kVK_ANSI_9, ouzel::input::Keyboard::Key::NUM_9},

    {kVK_ANSI_Comma, ouzel::input::Keyboard::Key::COMMA},
    {kVK_ANSI_Period, ouzel::input::Keyboard::Key::PERIOD},
    {kVK_PageUp, ouzel::input::Keyboard::Key::PAGE_UP},
    {kVK_PageDown, ouzel::input::Keyboard::Key::PAGE_DOWN},

    {kVK_ANSI_Keypad0, ouzel::input::Keyboard::Key::NUMPAD_0},
    {kVK_ANSI_Keypad1, ouzel::input::Keyboard::Key::NUMPAD_1},
    {kVK_ANSI_Keypad2, ouzel::input::Keyboard::Key::NUMPAD_2},
    {kVK_ANSI_Keypad3, ouzel::input::Keyboard::Key::NUMPAD_3},
    {kVK_ANSI_Keypad4, ouzel::input::Keyboard::Key::NUMPAD_4},
    {kVK_ANSI_Keypad5, ouzel::input::Keyboard::Key::NUMPAD_5},
    {kVK_ANSI_Keypad6, ouzel::input::Keyboard::Key::NUMPAD_6},
    {kVK_ANSI_Keypad7, ouzel::input::Keyboard::Key::NUMPAD_7},
    {kVK_ANSI_Keypad8, ouzel::input::Keyboard::Key::NUMPAD_8},
    {kVK_ANSI_Keypad9, ouzel::input::Keyboard::Key::NUMPAD_9},

    {kVK_ANSI_KeypadDecimal, ouzel::input::Keyboard::Key::NUMPAD_DECIMAL},
    {kVK_ANSI_KeypadMultiply, ouzel::input::Keyboard::Key::NUMPAD_MULTIPLY},
    {kVK_ANSI_KeypadPlus, ouzel::input::Keyboard::Key::NUMPAD_PLUS},
    {kVK_ANSI_KeypadClear, ouzel::input::Keyboard::Key::NUM_LOCK},
    {kVK_ANSI_KeypadDivide, ouzel::input::Keyboard::Key::NUMPAD_DIVIDE},
    {kVK_ANSI_KeypadEnter, ouzel::input::Keyboard::Key::NUMPAD_ENTER},
    {kVK_ANSI_KeypadMinus, ouzel::input::Keyboard::Key::NUMPAD_MINUS},

    {kVK_ANSI_Semicolon, ouzel::input::Keyboard::Key::SEMICOLON},
    {kVK_ANSI_Slash, ouzel::input::Keyboard::Key::SLASH},
    {kVK_ANSI_Grave, ouzel::input::Keyboard::Key::GRAVE},
    {kVK_ANSI_LeftBracket, ouzel::input::Keyboard::Key::LEFT_BRACKET},
    {kVK_ANSI_Backslash, ouzel::input::Keyboard::Key::BACKSLASH},
    {kVK_ANSI_RightBracket, ouzel::input::Keyboard::Key::RIGHT_BRACKET},
    {0x6E, ouzel::input::Keyboard::Key::MENU}
};

static ouzel::input::Keyboard::Key convertKeyCode(uint16_t keyCode)
{
    auto i = keyMap.find(keyCode);

    if (i != keyMap.end())
        return i->second;
    else
        return ouzel::input::Keyboard::Key::NONE;
}

-(void)keyDown:(NSEvent*)event
{
    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemMacOS->getKeyboardDevice();
    keyboardDevice->handleKeyPress(convertKeyCode(event.keyCode));
}

-(void)keyUp:(NSEvent*)event
{
    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemMacOS->getKeyboardDevice();
    keyboardDevice->handleKeyRelease(convertKeyCode(event.keyCode));
}

static const std::unordered_map<unsigned short, NSUInteger> maskMap = {
    {kVK_Control, NX_DEVICELCTLKEYMASK},
    {kVK_RightControl, NX_DEVICERCTLKEYMASK},
    {kVK_Command, NX_DEVICELCMDKEYMASK},
    {kVK_RightCommand, NX_DEVICERCMDKEYMASK},
    {kVK_Shift, NX_DEVICELSHIFTKEYMASK},
    {kVK_RightShift, NX_DEVICERSHIFTKEYMASK},
    {kVK_Option, NX_DEVICELALTKEYMASK},
    {kVK_RightOption, NX_DEVICERALTKEYMASK}
};

-(void)flagsChanged:(NSEvent*)event
{
    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemMacOS->getKeyboardDevice();

    auto i = maskMap.find(event.keyCode);
    if (i != maskMap.end())
    {
        NSUInteger mask = i->second;

        if (event.modifierFlags & mask)
            keyboardDevice->handleKeyPress(convertKeyCode(event.keyCode));
        else
            keyboardDevice->handleKeyRelease(convertKeyCode(event.keyCode));
    }
}

-(void)doCommandBySelector:(__unused SEL)selector
{
    // implement this method to not beep on Command-Escape
}

-(void)mouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleButtonPress(ouzel::input::Mouse::Button::LEFT,
                                   ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                static_cast<float>(location.y))));
}

-(void)mouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleButtonRelease(ouzel::input::Mouse::Button::LEFT,
                                     ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                  static_cast<float>(location.y))));
}

-(void)rightMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleButtonPress(ouzel::input::Mouse::Button::RIGHT,
                                   ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                static_cast<float>(location.y))));
}

-(void)rightMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleButtonRelease(ouzel::input::Mouse::Button::RIGHT,
                                     ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                  static_cast<float>(location.y))));
}

-(void)otherMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleButtonPress(ouzel::input::Mouse::Button::MIDDLE,
                                   ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                static_cast<float>(location.y))));
}

-(void)otherMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleButtonRelease(ouzel::input::Mouse::Button::MIDDLE,
                                     ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                  static_cast<float>(location.y))));
}

-(void)mouseMoved:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                         static_cast<float>(location.y))));
}

-(void)mouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                         static_cast<float>(location.y))));
}

-(void)rightMouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                         static_cast<float>(location.y))));
}

-(void)otherMouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                         static_cast<float>(location.y))));
}

-(void)scrollWheel:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDevice = inputSystemMacOS->getMouseDevice();
    mouseDevice->handleScroll(ouzel::Vector2(static_cast<float>(event.scrollingDeltaX),
                                             static_cast<float>(event.scrollingDeltaY)),
                              ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                           static_cast<float>(location.y))));
}

-(void)touchesBeganWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseBegan inView:self];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDevice* touchpadDevice = inputSystemMacOS->getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        CGPoint location = touch.normalizedPosition;

        touchpadDevice->handleTouchBegin(reinterpret_cast<uint64_t>(touch),
                                         ouzel::Vector2(static_cast<float>(location.x),
                                                        static_cast<float>(location.y)),
                                         1.0F);
    }
}

-(void)touchesMovedWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseMoved inView:self];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDevice* touchpadDevice = inputSystemMacOS->getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        CGPoint location = touch.normalizedPosition;

        touchpadDevice->handleTouchMove(reinterpret_cast<uint64_t>(touch),
                                        ouzel::Vector2(static_cast<float>(location.x),
                                                      static_cast<float>(location.y)),
                                        1.0F);
    }
}

-(void)touchesEndedWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseEnded inView:self];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDevice* touchpadDevice = inputSystemMacOS->getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        CGPoint location = touch.normalizedPosition;

        touchpadDevice->handleTouchEnd(reinterpret_cast<uint64_t>(touch),
                                       ouzel::Vector2(static_cast<float>(location.x),
                                                      static_cast<float>(location.y)),
                                       1.0F);
    }
}

-(void)touchesCancelledWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseCancelled inView:self];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDevice* touchpadDevice = inputSystemMacOS->getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        CGPoint location = touch.normalizedPosition;

        touchpadDevice->handleTouchCancel(reinterpret_cast<uint64_t>(touch),
                                          ouzel::Vector2(static_cast<float>(location.x),
                                                         static_cast<float>(location.y)),
                                          1.0F);
    }
}

-(void)swipeWithEvent:(__unused NSEvent*)event
{
}

-(void)rotateWithEvent:(__unused NSEvent*)event
{
}

-(void)magnifyWithEvent:(__unused NSEvent*)event
{
}

@end
