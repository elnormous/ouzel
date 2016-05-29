// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "MetalView.h"
#include "Engine.h"
#include "Window.h"
#include "InputApple.h"
#include "Utils.h"

using namespace ouzel;

@interface ViewDelegate: NSObject<MTKViewDelegate>

@end

@implementation ViewDelegate

-(void)mtkView:(nonnull MTKView*)view drawableSizeWillChange:(CGSize)size
{
    OUZEL_UNUSED(view);
    OUZEL_UNUSED(size);
}

-(void)drawInMTKView:(nonnull MTKView*)view
{
    if (!sharedEngine->run())
    {
        [view.window close];
    }
}

@end

@implementation MetalView
{
    id<MTKViewDelegate> viewDelegate;
}

-(id)initWithFrame:(CGRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        viewDelegate = [[ViewDelegate alloc] init];
        self.delegate = viewDelegate;
        running = YES;
    }

    return self;
}

-(void)dealloc
{
    [self close];
    [super dealloc];
}

-(void)close
{
    if (viewDelegate)
    {
        [viewDelegate release];
        viewDelegate = Nil;
        self.delegate = Nil;
    }

    running = NO;
}

-(void)changeDisplay
{

}

-(void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];

    if (!running) return;

    sharedEngine->getWindow()->setSize(Size2(static_cast<float>(newSize.width),
                                             static_cast<float>(newSize.height)));
}

-(BOOL)isFlipped
{
    return YES;
}

-(BOOL)acceptsFirstResponder
{
    return YES;
}

-(BOOL)acceptsFirstMouse:(NSEvent*)event
{
    OUZEL_UNUSED(event);
    return YES;
}

-(void)keyDown:(NSEvent*)event
{
    if (!running) return;

    sharedEngine->getInput()->keyDown(ouzel::input::InputApple::convertKeyCode(event.keyCode),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)keyUp:(NSEvent*)event
{
    if (!running) return;

    sharedEngine->getInput()->keyUp(ouzel::input::InputApple::convertKeyCode(event.keyCode),
                                    ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseDown:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseDown(input::MouseButton::LEFT,
                                        sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseUp:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseUp(input::MouseButton::LEFT,
                                      sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                static_cast<float>(location.y))),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseDown:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseDown(input::MouseButton::RIGHT,
                                        sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseUp:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseUp(input::MouseButton::RIGHT,
                                      sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                static_cast<float>(location.y))),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseDown:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseDown(input::MouseButton::MIDDLE,
                                        sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseUp:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseUp(input::MouseButton::MIDDLE,
                                      sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                static_cast<float>(location.y))),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseMoved:(NSEvent*)event
{
    if (!running) return;

    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseDragged:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)rightMouseDragged:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)otherMouseDragged:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)scrollWheel:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseScroll(Vector2(static_cast<float>(event.scrollingDeltaX),
                                                  static_cast<float>(event.scrollingDeltaY)),
                                          sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                    static_cast<float>(location.y))),
                                          ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)swipeWithEvent:(NSEvent*)event
{
    OUZEL_UNUSED(event);
}

-(void)rotateWithEvent:(NSEvent*)event
{
    OUZEL_UNUSED(event);
}

-(void)magnifyWithEvent:(NSEvent*)event
{
    OUZEL_UNUSED(event);
}

@end
