// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#import <Metal/Metal.h>
#include "MetalView.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "core/macos/WindowMacOS.h"
#include "input/macos/InputMacOS.h"
#include "graphics/metal/RendererMetal.h"

@implementation MetalView

-(id)initWithFrame:(NSRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        [self setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        [self setWantsLayer:YES];

        CAMetalLayer* metalLayer = [[[CAMetalLayer alloc] init] autorelease];

        CGFloat bgColor[] = { 0.0, 0.0, 0.0, 0.0 };
        metalLayer.edgeAntialiasingMask = 0;
        metalLayer.masksToBounds = YES;
        metalLayer.backgroundColor = CGColorCreate(CGColorSpaceCreateDeviceRGB(), bgColor);
        metalLayer.presentsWithTransaction = NO;
        metalLayer.anchorPoint = CGPointMake(0.5, 0.5);
        metalLayer.frame = frameRect;
        metalLayer.magnificationFilter = kCAFilterNearest;
        metalLayer.minificationFilter = kCAFilterNearest;
        metalLayer.framebufferOnly = NO;
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;

        [self setLayer:metalLayer];
    }

    return self;
}

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
    ouzel::sharedEngine->getInput()->keyDown(ouzel::input::InputMacOS::convertKeyCode(event.keyCode),
                                             ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)keyUp:(NSEvent*)event
{
    ouzel::sharedEngine->getInput()->keyUp(ouzel::input::InputMacOS::convertKeyCode(event.keyCode),
                                           ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)mouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseDown(ouzel::input::MouseButton::LEFT,
                                               ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                  static_cast<float>(location.y))),
                                               ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)mouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseUp(ouzel::input::MouseButton::LEFT,
                                             ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                static_cast<float>(location.y))),
                                             ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseDown(ouzel::input::MouseButton::RIGHT,
                                               ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                           static_cast<float>(location.y))),
                                               ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseUp(ouzel::input::MouseButton::RIGHT,
                                             ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                static_cast<float>(location.y))),
                                             ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseDown(ouzel::input::MouseButton::MIDDLE,
                                               ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                  static_cast<float>(location.y))),
                                               ouzel::input::InputMacOS::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseUp(ouzel::input::MouseButton::MIDDLE,
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

#endif
