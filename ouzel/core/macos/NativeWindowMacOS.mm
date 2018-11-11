// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "NativeWindowMacOS.hpp"
#include "ViewMacOS.h"
#include "graphics/RenderDevice.hpp"
#include "graphics/opengl/macos/OpenGLView.h"
#include "graphics/metal/macos/MetalView.h"
#include "utils/Errors.hpp"

@interface WindowDelegate: NSObject<NSWindowDelegate>
{
    ouzel::NativeWindowMacOS* window;
}

@end

@implementation WindowDelegate

-(id)initWithWindow:(ouzel::NativeWindowMacOS*)initWindow
{
    if (self = [super init])
        window = initWindow;

    return self;
}

-(void)windowDidResize:(__unused NSNotification*)notification
{
    window->handleResize();
}

-(void)windowWillClose:(__unused NSNotification*)notification
{
    window->handleClose();
}

-(void)windowDidEnterFullScreen:(__unused NSNotification*)notification
{
    window->handleFullscreenChange(true);
}

-(void)windowDidExitFullScreen:(__unused NSNotification*)notification
{
    window->handleFullscreenChange(false);
}

-(void)windowDidChangeBackingProperties:(__unused NSNotification*)notification
{
    window->handleScaleFactorChange();
}

-(void)windowDidChangeScreen:(__unused NSNotification*)notification
{
    window->handleScreenChange();
}

@end

namespace ouzel
{
    NativeWindowMacOS::NativeWindowMacOS(const std::function<void(const Event&)>& initCallback,
                                         const Size2& newSize,
                                         bool newResizable,
                                         bool newFullscreen,
                                         bool newExclusiveFullscreen,
                                         const std::string& newTitle,
                                         graphics::Driver graphicsDriver,
                                         bool newHighDpi):
        NativeWindow(initCallback,
                     newSize,
                     newResizable,
                     newFullscreen,
                     newExclusiveFullscreen,
                     newTitle,
                     newHighDpi)
    {
        screen = [NSScreen mainScreen];
        displayId = [[[screen deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];

        CGSize windowSize;

        if (highDpi)
        {
            windowSize.width = size.width;
            windowSize.height = size.height;
        }
        else
        {
            windowSize.width = round(size.width / screen.backingScaleFactor);
            windowSize.height = round(size.height / screen.backingScaleFactor);
        }

        if (windowSize.width <= 0.0F) windowSize.width = round(screen.frame.size.width * 0.8);
        if (windowSize.height <= 0.0F) windowSize.height = round(screen.frame.size.height * 0.8);

        NSRect frame = NSMakeRect(round(screen.frame.size.width / 2.0F - windowSize.width / 2.0F),
                                  round(screen.frame.size.height / 2.0F - windowSize.height / 2.0F),
                                  windowSize.width, windowSize.height);

        windowStyleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;

        if (resizable)
            windowStyleMask |= NSResizableWindowMask;

        window = [[NSWindow alloc] initWithContentRect:frame
                                             styleMask:windowStyleMask
                                               backing:NSBackingStoreBuffered
                                                 defer:NO
                                                screen:screen];
        [window setReleasedWhenClosed:NO];

        window.acceptsMouseMovedEvents = YES;
        windowDelegate = [[WindowDelegate alloc] initWithWindow:this];
        window.delegate = windowDelegate;

        [window setCollectionBehavior:exclusiveFullscreen ? NSWindowCollectionBehaviorFullScreenAuxiliary : NSWindowCollectionBehaviorFullScreenPrimary];

        if (exclusiveFullscreen)
        {
            if (fullscreen)
            {
                if (CGDisplayCapture(displayId) != kCGErrorSuccess)
                    throw SystemError("Failed to capture the main display");

                windowRect = frame;
                [window setStyleMask:NSBorderlessWindowMask];

                NSRect screenRect = [screen frame];
                [window setFrame:screenRect display:YES animate:NO];

                CGWindowLevel windowLevel = CGShieldingWindowLevel();
                [window setLevel:windowLevel];
            }
        }
        else
        {
            if (fullscreen)
                [window toggleFullScreen:nil];
        }

        [window setTitle:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:title.c_str()])];

        NSRect windowFrame = [NSWindow contentRectForFrameRect:[window frame]
                                                     styleMask:[window styleMask]];

        switch (graphicsDriver)
        {
            case graphics::Driver::EMPTY:
                view = [[ViewMacOS alloc] initWithFrame:windowFrame];
                break;
#if OUZEL_COMPILE_OPENGL
            case graphics::Driver::OPENGL:
                view = [[OpenGLView alloc] initWithFrame:windowFrame];
                [view setWantsBestResolutionOpenGLSurface:highDpi ? YES : NO];
                break;
#endif
#if OUZEL_COMPILE_METAL
            case graphics::Driver::METAL:
                view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
#endif
            default:
                throw SystemError("Unsupported render driver");
        }

        [view setAcceptsTouchEvents:YES];
        [view setWantsRestingTouches:YES];

        window.contentView = view;
        [window makeKeyAndOrderFront:nil];

        size.width = static_cast<float>(windowSize.width);
        size.height = static_cast<float>(windowSize.height);

        if (highDpi)
        {
            contentScale = static_cast<float>(screen.backingScaleFactor);
            resolution = size * contentScale;
        }
        else
        {
            contentScale = 1.0F;
            resolution = size;
        }
    }

    NativeWindowMacOS::~NativeWindowMacOS()
    {
        if (exclusiveFullscreen && fullscreen)
            CGDisplayRelease(displayId);

        if (view) [view release];

        if (window)
        {
            window.delegate = nil;
            [window close];
            [window release];
        }

        if (windowDelegate) [windowDelegate release];
    }

    void NativeWindowMacOS::close()
    {
        NativeWindow::close();

        if (view)
        {
            [view release];
            view = nil;
        }

        if (window)
        {
            window.delegate = nil;
            [window close];
            [window release];
            window = nil;
        }

        if (windowDelegate)
        {
            [windowDelegate release];
            windowDelegate = nil;
        }
    }

    void NativeWindowMacOS::setSize(const Size2& newSize)
    {
        NativeWindow::setSize(newSize);

        NSRect frame = [window frame];

        NSRect newFrame = [NSWindow frameRectForContentRect:NSMakeRect(NSMinX(frame), NSMinY(frame), newSize.width, newSize.height)
                                                  styleMask:[window styleMask]];

        if (frame.size.width != newFrame.size.width ||
            frame.size.height != newFrame.size.height)
        {
            [window setFrame:newFrame display:YES animate:NO];
            resolution = size * contentScale;

            postEvent(Event(Event::Type::RESOLUTION_CHANGE, resolution));
        }
    }

    void NativeWindowMacOS::setFullscreen(bool newFullscreen)
    {
        if (fullscreen != newFullscreen)
        {
            if (exclusiveFullscreen)
            {
                if (newFullscreen)
                {
                    if (CGDisplayCapture(displayId) != kCGErrorSuccess)
                        throw SystemError("Failed to capture the main display");

                    windowRect = [window frame];
                    [window setStyleMask:NSBorderlessWindowMask];

                    NSRect screenRect = [screen frame];
                    [window setFrame:screenRect display:YES animate:NO];

                    CGWindowLevel windowLevel = CGShieldingWindowLevel();
                    [window setLevel:windowLevel];
                    [window makeKeyAndOrderFront:nil];
                }
                else
                {
                    [window setStyleMask:windowStyleMask];
                    [window setFrame:windowRect display:YES animate:NO];

                    if (CGDisplayRelease(displayId) != kCGErrorSuccess)
                        throw SystemError("Failed to release the main display");
                }
            }
            else
            {
                NSApplicationPresentationOptions options = [[NSApplication sharedApplication] presentationOptions];
                bool isFullscreen = (options & NSApplicationPresentationFullScreen) > 0;

                if (isFullscreen != newFullscreen)
                    [window toggleFullScreen:nil];
            }
        }

        NativeWindow::setFullscreen(newFullscreen);
    }

    void NativeWindowMacOS::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            NSString* objCTitle = [NSString stringWithCString:newTitle.c_str() encoding:NSUTF8StringEncoding];

            window.title = objCTitle;
        }

        NativeWindow::setTitle(newTitle);
    }

    void NativeWindowMacOS::handleResize()
    {
        NSRect frame = [NSWindow contentRectForFrameRect:window.frame
                                               styleMask:window.styleMask];

        size = Size2(static_cast<float>(frame.size.width),
                     static_cast<float>(frame.size.height));
        resolution = size * contentScale;

        postEvent(Event(Event::Type::SIZE_CHANGE, size));
        postEvent(Event(Event::Type::RESOLUTION_CHANGE, resolution));
    }

    void NativeWindowMacOS::handleClose()
    {
        postEvent(Event(Event::Type::CLOSE));
    }

    void NativeWindowMacOS::handleFullscreenChange(bool newFullscreen)
    {
        fullscreen = newFullscreen;

        postEvent(Event(Event::Type::FULLSCREEN_CHANGE, fullscreen));
    }

    void NativeWindowMacOS::handleScaleFactorChange()
    {
        if (highDpi)
        {
            contentScale = static_cast<float>(window.backingScaleFactor);
            resolution = size * contentScale;

            postEvent(Event(Event::Type::RESOLUTION_CHANGE, resolution));
        }
    }

    void NativeWindowMacOS::handleScreenChange()
    {
        screen = [window screen];
        displayId = [[[screen deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];

        postEvent(Event(Event::Type::SCREEN_CHANGE, displayId));
    }
}
