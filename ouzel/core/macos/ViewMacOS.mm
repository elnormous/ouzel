// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "ViewMacOS.h"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "input/macos/InputMacOS.hpp"

@implementation ViewMacOS

-(BOOL)isOpaque
{
    return YES;
}

-(BOOL)mouseDownCanMoveWindow
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
    [self discardCursorRects];

    ouzel::input::InputMacOS* inputMacOS = static_cast<ouzel::input::InputMacOS*>(ouzel::sharedEngine->getInput());

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
    ouzel::sharedEngine->getInput()->keyPress(ouzel::input::InputMacOS::convertKeyCode(event.keyCode),
                                              ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)keyUp:(NSEvent*)event
{
    ouzel::sharedEngine->getInput()->keyRelease(ouzel::input::InputMacOS::convertKeyCode(event.keyCode),
                                                ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)flagsChanged:(NSEvent*)event
{
    if (NSUInteger mask = ouzel::input::InputMacOS::getKeyMask(event.keyCode))
    {
        if (event.modifierFlags & mask)
        {
            ouzel::sharedEngine->getInput()->keyPress(ouzel::input::InputMacOS::convertKeyCode(event.keyCode),
                                                      ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
        }
        else
        {
            ouzel::sharedEngine->getInput()->keyRelease(ouzel::input::InputMacOS::convertKeyCode(event.keyCode),
                                                        ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
        }
    }
}

-(void)doCommandBySelector:(__unused SEL)selector
{
    // implement this method to not beep on Command-Escape
}

-(void)mouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseButtonPress(ouzel::input::MouseButton::LEFT,
                                                      ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                         static_cast<float>(location.y))),
                                                      ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)mouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseButtonRelease(ouzel::input::MouseButton::LEFT,
                                                        ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                           static_cast<float>(location.y))),
                                                        ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseButtonPress(ouzel::input::MouseButton::RIGHT,
                                                      ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                         static_cast<float>(location.y))),
                                                      ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseButtonRelease(ouzel::input::MouseButton::RIGHT,
                                                        ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                           static_cast<float>(location.y))),
                                                        ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseButtonPress(ouzel::input::MouseButton::MIDDLE,
                                                      ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                         static_cast<float>(location.y))),
                                                      ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseButtonRelease(ouzel::input::MouseButton::MIDDLE,
                                                        ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                           static_cast<float>(location.y))),
                                                        ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)mouseMoved:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseMove(ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                  static_cast<float>(location.y))),
                                               ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)mouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseMove(ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                  static_cast<float>(location.y))),
                                               ouzel::input::InputMacOS::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)rightMouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseMove(ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                  static_cast<float>(location.y))),
                                               ouzel::input::InputMacOS::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)otherMouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseMove(ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                  static_cast<float>(location.y))),
                                               ouzel::input::InputMacOS::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)scrollWheel:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseScroll(ouzel::Vector2(static_cast<float>(event.scrollingDeltaX),
                                                                static_cast<float>(event.scrollingDeltaY)),
                                                 ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                    static_cast<float>(location.y))),
                                                 ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
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
