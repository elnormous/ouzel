// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "OpenGLView.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "RendererOGLMacOS.h"
#include "core/Window.h"
#include "input/apple/InputApple.h"
#include "utils/Utils.h"

static CVReturn renderCallback(CVDisplayLinkRef,
                               const CVTimeStamp*,
                               const CVTimeStamp*,
                               CVOptionFlags,
                               CVOptionFlags*,
                               void*)
{
    @autoreleasepool
    {
        if (ouzel::sharedEngine->isRunning() && !ouzel::sharedEngine->draw())
        {
            ouzel::sharedApplication->execute([] {
                ouzel::sharedEngine->getWindow()->close();
            });
        }
    }

    return kCVReturnSuccess;
}

@implementation OpenGLView

-(void)dealloc
{
    if (displayLink)
    {
        CVDisplayLinkStop(displayLink);
        CVDisplayLinkRelease(displayLink);
    }

    [[NSNotificationCenter defaultCenter] removeObserver:self];

    [super dealloc];
}

-(void)viewDidMoveToWindow
{
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(windowWillClose:)
                                                 name:NSWindowWillCloseNotification
                                               object:_window];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(windowDiChangeScreen:)
                                                 name:NSWindowDidChangeScreenNotification
                                               object:_window];
}

-(void)windowWillClose:(__unused NSNotification*)notification
{
    if (displayLink)
    {
        CVDisplayLinkStop(displayLink);
        CVDisplayLinkRelease(displayLink);
        displayLink = Nil;
    }

    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

-(void)windowDiChangeScreen:(__unused NSNotification*)notification
{
    [self createDisplayLink];
}

-(BOOL)isFlipped
{
    return YES;
}

-(BOOL)acceptsFirstMouse:(__unused NSEvent*)event
{
    return YES;
}

-(void)createDisplayLink
{
    if (displayLink)
    {
        CVDisplayLinkStop(displayLink);
        CVDisplayLinkRelease(displayLink);
        displayLink = Nil;
    }

    NSScreen* screen = [_window screen];
    displayId = (CGDirectDisplayID)[[[screen deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];

    if (!displayId) displayId = CGMainDisplayID();

    CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, renderCallback, nullptr);

    CVDisplayLinkStart(displayLink);
}

-(void)prepareOpenGL
{
    [self setWantsBestResolutionOpenGLSurface:YES];

    [self createDisplayLink];
}

-(void)lockFocus
{
    [super lockFocus];

    ouzel::graphics::RendererOGLMacOS* rendererOGL = static_cast<ouzel::graphics::RendererOGLMacOS*>(ouzel::sharedEngine->getRenderer());

    NSOpenGLContext* openGLContext = rendererOGL->getOpenGLContext();

    if (openGLContext.view != self)
    {
        [openGLContext setView:self];
    }

    [openGLContext makeCurrentContext];
}

-(BOOL)acceptsFirstResponder
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
