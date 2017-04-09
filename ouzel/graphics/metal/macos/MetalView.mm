// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <Metal/Metal.h>
#include "MetalView.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "input/apple/InputApple.h"
#include "graphics/metal/RendererMetal.h"
#include "utils/Utils.h"

@implementation MetalView

-(id)initWithFrame:(NSRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        [self setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        [self setWantsLayer:YES];

        _metalLayer = [[CAMetalLayer alloc] init];

        CGFloat bgColor[] = { 0.0, 0.0, 0.0, 0.0 };
        _metalLayer.edgeAntialiasingMask = 0;
        _metalLayer.masksToBounds = YES;
        _metalLayer.backgroundColor = CGColorCreate(CGColorSpaceCreateDeviceRGB(), bgColor);
        _metalLayer.presentsWithTransaction = NO;
        _metalLayer.anchorPoint = CGPointMake(0.5, 0.5);
        _metalLayer.frame = frameRect;
        _metalLayer.magnificationFilter = kCAFilterNearest;
        _metalLayer.minificationFilter = kCAFilterNearest;
        _metalLayer.framebufferOnly = NO;

        [self setLayer:_metalLayer];
    }

    return self;
}

-(BOOL)isOpaque
{
    return YES;
}

- (BOOL)mouseDownCanMoveWindow
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

-(void)keyDown:(NSEvent*)event
{
    ouzel::sharedEngine->getInput()->keyDown(ouzel::input::InputApple::convertKeyCode(event.keyCode),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)keyUp:(NSEvent*)event
{
    ouzel::sharedEngine->getInput()->keyUp(ouzel::input::InputApple::convertKeyCode(event.keyCode),
                                    ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseDown(ouzel::input::MouseButton::LEFT,
                                               ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                  static_cast<float>(location.y))),
                                               ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseUp(ouzel::input::MouseButton::LEFT,
                                             ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                static_cast<float>(location.y))),
                                             ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseDown(ouzel::input::MouseButton::RIGHT,
                                               ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                           static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseUp(ouzel::input::MouseButton::RIGHT,
                                             ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                static_cast<float>(location.y))),
                                             ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseDown(ouzel::input::MouseButton::MIDDLE,
                                               ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                  static_cast<float>(location.y))),
                                               ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseUp(ouzel::input::MouseButton::MIDDLE,
                                             ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                static_cast<float>(location.y))),
                                             ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseMoved:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseMove(ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                  static_cast<float>(location.y))),
                                               ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseMove(ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                  static_cast<float>(location.y))),
                                               ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)rightMouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseMove(ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                  static_cast<float>(location.y))),
                                               ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)otherMouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseMove(ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                  static_cast<float>(location.y))),
                                               ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)scrollWheel:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    ouzel::sharedEngine->getInput()->mouseScroll(ouzel::Vector2(static_cast<float>(event.scrollingDeltaX),
                                                                static_cast<float>(event.scrollingDeltaY)),
                                                 ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                    static_cast<float>(location.y))),
                                                 ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
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
