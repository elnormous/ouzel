// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#import "ViewMacOS.h"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "input/macos/InputManagerMacOS.hpp"
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

    ouzel::input::InputManagerMacOS* inputMacOS = static_cast<ouzel::input::InputManagerMacOS*>(ouzel::engine->getInputManager());

    if (inputMacOS->isCursorVisible())
    {
        [self addCursorRect:[self bounds] cursor:inputMacOS->getNativeCursor()];
        [inputMacOS->getNativeCursor() set];
    }
    else
    {
        [self addCursorRect:[self bounds] cursor:inputMacOS->getEmptyCursor()];
        [inputMacOS->getEmptyCursor() set];
    }
}

-(void)keyDown:(NSEvent*)event
{
    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDeviceMacOS* keyboardDeviceMacOS = inputSystemMacOS->getKeyboardDevice();
    keyboardDeviceMacOS->handleKeyPress(ouzel::input::InputSystemMacOS::convertKeyCode(event.keyCode),
                                        ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)keyUp:(NSEvent*)event
{
    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDeviceMacOS* keyboardDeviceMacOS = inputSystemMacOS->getKeyboardDevice();
    keyboardDeviceMacOS->handleKeyRelease(ouzel::input::InputSystemMacOS::convertKeyCode(event.keyCode),
                                          ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)flagsChanged:(NSEvent*)event
{
    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDeviceMacOS* keyboardDeviceMacOS = inputSystemMacOS->getKeyboardDevice();
    if (NSUInteger mask = ouzel::input::InputSystemMacOS::getKeyMask(event.keyCode))
    {
        if (event.modifierFlags & mask)
            keyboardDeviceMacOS->handleKeyPress(ouzel::input::InputSystemMacOS::convertKeyCode(event.keyCode),
                                                ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, 0));
        else
            keyboardDeviceMacOS->handleKeyRelease(ouzel::input::InputSystemMacOS::convertKeyCode(event.keyCode),
                                                  ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, 0));
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
    ouzel::input::MouseDeviceMacOS* mouseDeviceMacOS = inputSystemMacOS->getMouseDevice();
    mouseDeviceMacOS->handleButtonPress(ouzel::input::Mouse::Button::LEFT,
                                        ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                     static_cast<float>(location.y))),
                                        ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)mouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDeviceMacOS = inputSystemMacOS->getMouseDevice();
    mouseDeviceMacOS->handleButtonRelease(ouzel::input::Mouse::Button::LEFT,
                                          ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                       static_cast<float>(location.y))),
                                          ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDeviceMacOS = inputSystemMacOS->getMouseDevice();
    mouseDeviceMacOS->handleButtonPress(ouzel::input::Mouse::Button::RIGHT,
                                        ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                     static_cast<float>(location.y))),
                                        ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDeviceMacOS = inputSystemMacOS->getMouseDevice();
    mouseDeviceMacOS->handleButtonRelease(ouzel::input::Mouse::Button::RIGHT,
                                          ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                       static_cast<float>(location.y))),
                                          ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDeviceMacOS = inputSystemMacOS->getMouseDevice();
    mouseDeviceMacOS->handleButtonPress(ouzel::input::Mouse::Button::MIDDLE,
                                        ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                     static_cast<float>(location.y))),
                                        ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDeviceMacOS = inputSystemMacOS->getMouseDevice();
    mouseDeviceMacOS->handleButtonRelease(ouzel::input::Mouse::Button::MIDDLE,
                                          ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                       static_cast<float>(location.y))),
                                          ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)mouseMoved:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDeviceMacOS = inputSystemMacOS->getMouseDevice();
    mouseDeviceMacOS->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                              static_cast<float>(location.y))),
                                 ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)mouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDeviceMacOS = inputSystemMacOS->getMouseDevice();
    mouseDeviceMacOS->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                              static_cast<float>(location.y))),
                                 ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)rightMouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDeviceMacOS = inputSystemMacOS->getMouseDevice();
    mouseDeviceMacOS->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                              static_cast<float>(location.y))),
                                 ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)otherMouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDeviceMacOS = inputSystemMacOS->getMouseDevice();
    mouseDeviceMacOS->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                              static_cast<float>(location.y))),
                                 ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)scrollWheel:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::MouseDeviceMacOS* mouseDeviceMacOS = inputSystemMacOS->getMouseDevice();
    mouseDeviceMacOS->handleScroll(ouzel::Vector2(static_cast<float>(event.scrollingDeltaX),
                                                  static_cast<float>(event.scrollingDeltaY)),
                                   ouzel::engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                static_cast<float>(location.y))),
                                   ouzel::input::InputSystemMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)touchesBeganWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseBegan inView:self];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDeviceMacOS* touchpadDeviceMacOS = inputSystemMacOS->getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        CGPoint location = touch.normalizedPosition;

        touchpadDeviceMacOS->handleTouchBegin(reinterpret_cast<uint64_t>(touch),
                                              ouzel::Vector2(static_cast<float>(location.x),
                                                             static_cast<float>(location.y)),
                                              1.0F);
    }
}

-(void)touchesMovedWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseMoved inView:self];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDeviceMacOS* touchpadDeviceMacOS = inputSystemMacOS->getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        CGPoint location = touch.normalizedPosition;

        touchpadDeviceMacOS->handleTouchMove(reinterpret_cast<uint64_t>(touch),
                                             ouzel::Vector2(static_cast<float>(location.x),
                                                            static_cast<float>(location.y)),
                                             1.0F);
    }
}

-(void)touchesEndedWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseEnded inView:self];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDeviceMacOS* touchpadDeviceMacOS = inputSystemMacOS->getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        CGPoint location = touch.normalizedPosition;

        touchpadDeviceMacOS->handleTouchEnd(reinterpret_cast<uint64_t>(touch),
                                            ouzel::Vector2(static_cast<float>(location.x),
                                                           static_cast<float>(location.y)),
                                            1.0F);
    }
}

-(void)touchesCancelledWithEvent:(NSEvent*)event
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseCancelled inView:self];

    ouzel::input::InputSystemMacOS* inputSystemMacOS = static_cast<ouzel::input::InputSystemMacOS*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::TouchpadDeviceMacOS* touchpadDeviceMacOS = inputSystemMacOS->getTouchpadDevice();
    for (NSTouch* touch in touches)
    {
        CGPoint location = touch.normalizedPosition;

        touchpadDeviceMacOS->handleTouchCancel(reinterpret_cast<uint64_t>(touch),
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
