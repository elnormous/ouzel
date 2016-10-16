// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "OpenGLView.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "RendererOGLMacOS.h"
#include "core/Window.h"
#include "input/apple/InputApple.h"
#include "utils/Utils.h"

using namespace ouzel;

static CVReturn renderCallback(CVDisplayLinkRef,
                               const CVTimeStamp*,
                               const CVTimeStamp*,
                               CVOptionFlags,
                               CVOptionFlags*,
                               void*)
{
    @autoreleasepool
    {
        if (!sharedEngine->isActive() ||
            (sharedEngine->isRunning() && !sharedEngine->draw()))
        {
            sharedApplication->execute([] {
                sharedEngine->getWindow()->close();
            });
        }
    }

    return kCVReturnSuccess;
}

@implementation OpenGLView

-(id)initWithFrame:(NSRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(windowWillClose:)
                                                     name:NSWindowWillCloseNotification
                                                   object:nil];
    }

    return self;
}

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

-(void)windowWillClose:(NSNotification*)notification
{
    if (notification.object == self.window)
    {
        if (displayLink)
        {
            CVDisplayLinkStop(displayLink);
            CVDisplayLinkRelease(displayLink);
            displayLink = Nil;
        }

        [[NSNotificationCenter defaultCenter] removeObserver:self];
    }
}

-(BOOL)isFlipped
{
    return YES;
}

-(BOOL)acceptsFirstMouse:(__unused NSEvent*)event
{
    return YES;
}

-(void)viewDidChangeBackingProperties
{
    [super viewDidChangeBackingProperties];

    sharedEngine->getRenderer()->setSize(Size2(static_cast<float>(_bounds.size.width * _window.backingScaleFactor),
                                               static_cast<float>(_bounds.size.height * _window.backingScaleFactor)));
}

-(void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];

    sharedEngine->getRenderer()->setSize(Size2(static_cast<float>(_bounds.size.width * _window.backingScaleFactor),
                                               static_cast<float>(_bounds.size.height * _window.backingScaleFactor)));
}

-(void)prepareOpenGL
{
    [self setWantsBestResolutionOpenGLSurface:YES];

    NSScreen* screen = [_window screen];
    displayId = (CGDirectDisplayID)[[[screen deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];

    if (!displayId) displayId = CGMainDisplayID();

    graphics::RendererOGLMacOS* rendererOGL = static_cast<graphics::RendererOGLMacOS*>(sharedEngine->getRenderer());

    NSOpenGLContext* openGLContext = rendererOGL->getOpenGLContext();

    [openGLContext setView:self];
    [openGLContext makeCurrentContext];

    GLint swapInt = sharedEngine->getSettings().verticalSync ? 1 : 0;
    [openGLContext setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

    CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, renderCallback, nullptr);

    CVDisplayLinkStart(displayLink);
}

-(void)lockFocus
{
    [super lockFocus];

    graphics::RendererOGLMacOS* rendererOGL = static_cast<graphics::RendererOGLMacOS*>(sharedEngine->getRenderer());

    NSOpenGLContext* openGLContext = rendererOGL->getOpenGLContext();

    [openGLContext setView:self];
    [openGLContext makeCurrentContext];
}

-(BOOL)acceptsFirstResponder
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
                                        sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(location.x),
                                                                                                             static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseUp(input::MouseButton::LEFT,
                                      sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(location.x),
                                                                                                           static_cast<float>(location.y))),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseDown(input::MouseButton::RIGHT,
                                        sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(location.x),
                                                                                                             static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)rightMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseUp(input::MouseButton::RIGHT,
                                      sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(location.x),
                                                                                                           static_cast<float>(location.y))),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseDown:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseDown(input::MouseButton::MIDDLE,
                                        sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(location.x),
                                                                                                             static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)otherMouseUp:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseUp(input::MouseButton::MIDDLE,
                                      sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(location.x),
                                                                                                           static_cast<float>(location.y))),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseMoved:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseMove(sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(location.x),
                                                                                                             static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
}

-(void)mouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseMove(sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(location.x),
                                                                                                             static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)rightMouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseMove(sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(location.x),
                                                                                                             static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)otherMouseDragged:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseMove(sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(location.x),
                                                                                                             static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
}

-(void)scrollWheel:(NSEvent*)event
{
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    sharedEngine->getInput()->mouseScroll(Vector2(static_cast<float>(event.scrollingDeltaX),
                                                  static_cast<float>(event.scrollingDeltaY)),
                                          sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(location.x),
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
