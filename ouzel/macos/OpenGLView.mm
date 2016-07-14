// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "OpenGLView.h"
#include "core/Engine.h"
#include "opengl/RendererOGL.h"
#include "core/Window.h"
#include "apple/InputApple.h"
#include "utils/Utils.h"

static CVReturn renderCallback(CVDisplayLinkRef,
                               const CVTimeStamp*,
                               const CVTimeStamp*,
                               CVOptionFlags,
                               CVOptionFlags*,
                               void* displayLinkContext)
{
    [(__bridge OpenGLView*)displayLinkContext draw];

    return kCVReturnSuccess;
}

using namespace ouzel;

@implementation OpenGLView

-(id)initWithFrame:(NSRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        std::shared_ptr<graphics::RendererOGL> rendererOGL = std::static_pointer_cast<graphics::RendererOGL>(sharedEngine->getRenderer());

        // Create pixel format
        NSOpenGLPixelFormatAttribute openGL3Attributes[] =
        {
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADepthSize, 32, // set depth buffer size
            0
        };

        NSOpenGLPixelFormat* pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:openGL3Attributes] autorelease];

        if (pixelFormat)
        {
            rendererOGL->setAPIVersion(3);
            log("Using OpenGL 3.2");
        }
        else
        {
            log("Failed to crete OpenGL 3.2 pixel format");

            NSOpenGLPixelFormatAttribute openGL2Attributes[] =
            {
                NSOpenGLPFADoubleBuffer,
                NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
                NSOpenGLPFAColorSize, 24,
                NSOpenGLPFAAlphaSize, 8,
                NSOpenGLPFADepthSize, 32, // set depth buffer size
                0
            };

            pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:openGL2Attributes] autorelease];

            if (pixelFormat)
            {
                rendererOGL->setAPIVersion(2);
                log("Using OpenGL 2");
            }
        }

        if (!pixelFormat)
        {
            log("Failed to crete OpenGL 2 pixel format");
            return Nil;
        }

        // Create OpenGL context
        openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:NULL];
        [openGLContext setView:self];
        [openGLContext makeCurrentContext];

        GLint swapInt = sharedEngine->getSettings().verticalSync ? 1 : 0;
        [openGLContext setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

        displayId = (CGDirectDisplayID)[[[[_window screen] deviceDescription]objectForKey:@"NSScreenNumber"] unsignedIntValue];

        CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink);
        CVDisplayLinkSetOutputCallback(displayLink, renderCallback, (__bridge void *)self);

        CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, [openGLContext CGLContextObj], [pixelFormat CGLPixelFormatObj]);

        CVDisplayLinkStart(displayLink);
    }

    return self;
}

-(void)dealloc
{
    [self close];

    [super dealloc];
}

-(BOOL)isFlipped
{
    return YES;
}

-(BOOL)acceptsFirstMouse:(NSEvent*)event
{
    OUZEL_UNUSED(event);
    return YES;
}

-(void)prepareOpenGL
{
    running = YES;
}

-(void)close
{
    running = NO;

    if (displayLink)
    {
        CGLLockContext([openGLContext CGLContextObj]);
        [openGLContext makeCurrentContext];

        CVDisplayLinkRelease(displayLink);
        displayLink = Nil;

        CGLUnlockContext([openGLContext CGLContextObj]);
    }

    if (openGLContext)
    {
        [openGLContext release];
        openGLContext = Nil;
    }
}

-(void)changeDisplay
{
    CGDirectDisplayID currentDisplayId = (CGDirectDisplayID)[[[[_window screen] deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];

    if(displayId != 0 && displayId != currentDisplayId)
    {
        CVDisplayLinkSetCurrentCGDisplay(displayLink, currentDisplayId);
        displayId = currentDisplayId;
    }
}

-(void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];

    if (!running) return;

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];

    [openGLContext update];

    sharedEngine->getWindow()->setSize(Size2(static_cast<float>(newSize.width),
                                             static_cast<float>(newSize.height)));

    CGLUnlockContext([openGLContext CGLContextObj]);
}

-(void)lockFocus
{
    [super lockFocus];
    if ([openGLContext view] != self)
    {
        [openGLContext setView:self];
    }

    [openGLContext makeCurrentContext];
}

-(void)draw
{
    if (!running) return;

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];

    bool quit = !sharedEngine->run();

    [openGLContext flushBuffer];

    CGLUnlockContext([openGLContext CGLContextObj]);

    if (quit)
    {
        if ([NSThread isMainThread])
        {
            [self.window close];
        }
        else
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                [self.window close];
            });
        }
    }
}

-(BOOL)acceptsFirstResponder
{
    return YES;
}

-(void)keyDown:(NSEvent*)event
{
    if (!running) return;

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];
    sharedEngine->getInput()->keyDown(ouzel::input::InputApple::convertKeyCode(event.keyCode),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
    CGLUnlockContext([openGLContext CGLContextObj]);
}

-(void)keyUp:(NSEvent*)event
{
    if (!running) return;

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];
    sharedEngine->getInput()->keyUp(ouzel::input::InputApple::convertKeyCode(event.keyCode),
                                    ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
    CGLUnlockContext([openGLContext CGLContextObj]);
}

-(void)mouseDown:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseDown(input::MouseButton::LEFT,
                                                 sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                    static_cast<float>(location.y))),
                                                 ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
    CGLUnlockContext([openGLContext CGLContextObj]);
}

-(void)mouseUp:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseUp(input::MouseButton::LEFT,
                                               sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                  static_cast<float>(location.y))),
                                               ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
    CGLUnlockContext([openGLContext CGLContextObj]);
}

-(void)rightMouseDown:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseDown(input::MouseButton::RIGHT,
                                                 sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                                    static_cast<float>(location.y))),
                                                 ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
    CGLUnlockContext([openGLContext CGLContextObj]);
}

-(void)rightMouseUp:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseUp(input::MouseButton::RIGHT,
                                      sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                static_cast<float>(location.y))),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
    CGLUnlockContext([openGLContext CGLContextObj]);
}

-(void)otherMouseDown:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseDown(input::MouseButton::MIDDLE,
                                        sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
    CGLUnlockContext([openGLContext CGLContextObj]);
}

-(void)otherMouseUp:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseUp(input::MouseButton::MIDDLE,
                                      sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                static_cast<float>(location.y))),
                                      ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
    CGLUnlockContext([openGLContext CGLContextObj]);
}

-(void)mouseMoved:(NSEvent*)event
{
    if (!running) return;

    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
    CGLUnlockContext([openGLContext CGLContextObj]);
}

-(void)mouseDragged:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
    CGLUnlockContext([openGLContext CGLContextObj]);
}

-(void)rightMouseDragged:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
    CGLUnlockContext([openGLContext CGLContextObj]);
}

-(void)otherMouseDragged:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                  static_cast<float>(location.y))),
                                        ouzel::input::InputApple::getModifiers(event.modifierFlags, NSEvent.pressedMouseButtons));
    CGLUnlockContext([openGLContext CGLContextObj]);
}

-(void)scrollWheel:(NSEvent*)event
{
    if (!running) return;
    NSPoint location = [self convertPoint:event.locationInWindow fromView: nil];

    CGLLockContext([openGLContext CGLContextObj]);
    [openGLContext makeCurrentContext];
    sharedEngine->getInput()->mouseScroll(Vector2(static_cast<float>(event.scrollingDeltaX),
                                                  static_cast<float>(event.scrollingDeltaY)),
                                          sharedEngine->getRenderer()->viewToScreenLocation(Vector2(static_cast<float>(location.x),
                                                                                                    static_cast<float>(location.y))),
                                          ouzel::input::InputApple::getModifiers(event.modifierFlags, 0));
    CGLUnlockContext([openGLContext CGLContextObj]);
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
