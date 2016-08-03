// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "MetalView.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "apple/InputApple.h"
#include "utils/Utils.h"

using namespace ouzel;

@interface ViewDelegate: NSObject<MTKViewDelegate>

@end

@implementation ViewDelegate

-(void)mtkView:(nonnull __unused MTKView*)view drawableSizeWillChange:(__unused CGSize)size
{
}

-(void)drawInMTKView:(nonnull MTKView*)view
{
    if (sharedEngine->isRunning())
    {
        if (!sharedEngine->draw())
        {
            sharedEngine->exit();
        }
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
    }

    return self;
}

-(void)dealloc
{
    if (viewDelegate)
    {
        [viewDelegate release];
        viewDelegate = Nil;
        self.delegate = Nil;
    }

    [super dealloc];
}

-(void)changeDisplay
{

}

-(void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];

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

-(BOOL)acceptsFirstMouse:(__unused NSEvent*)event
{
    return YES;
}

-(void)keyDown:(NSEvent*)event
{
    sharedEngine->getInput()->keyDown(ouzel::input::InputApple::convertKeyCode(event.keyCode),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)keyUp:(NSEvent*)event
{
    sharedEngine->getInput()->keyUp(ouzel::input::InputApple::convertKeyCode(event.keyCode),
                                    ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseDown(input::MouseButton::LEFT,
                                        sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseUp(input::MouseButton::LEFT,
                                      sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                static_cast<float>(location.y))),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseDown(input::MouseButton::RIGHT,
                                        sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseUp(input::MouseButton::RIGHT,
                                      sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                static_cast<float>(location.y))),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseDown(input::MouseButton::MIDDLE,
                                        sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseUp(input::MouseButton::MIDDLE,
                                      sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                static_cast<float>(location.y))),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseMoved:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)rightMouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)otherMouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)scrollWheel:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseScroll(Vector2(static_cast<float>(event.scrollingDeltaX),
                                                  static_cast<float>(event.scrollingDeltaY)),
                                          sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
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
