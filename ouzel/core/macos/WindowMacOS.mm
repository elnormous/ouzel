// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowMacOS.h"
#include "graphics/opengl/macos/OpenGLView.h"
#include "graphics/metal/macos/MetalView.h"
#include "input/macos/InputMacOS.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "utils/Log.h"

@interface WindowDelegate: NSObject<NSWindowDelegate>
{
    ouzel::WindowMacOS* window;
}

@end

@implementation WindowDelegate

-(void)handleQuit:(__unused id)sender
{
    window->close();
}

-(id)initWithWindow:(ouzel::WindowMacOS*)newWindow
{
    if (self = [super init])
    {
        window = newWindow;
    }

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
    WindowMacOS::WindowMacOS()
    {
    }

    WindowMacOS::~WindowMacOS()
    {
        if (view)
        {
            [view release];
        }

        if (window)
        {
            window.delegate = Nil;
            [window close];
            [window release];
        }

        if (windowDelegate)
        {
            [windowDelegate release];
        }
    }

    bool WindowMacOS::init(const Size2& newSize,
                           bool newResizable,
                           bool newFullscreen,
                           const std::string& newTitle,
                           bool newHighDpi,
                           bool depth)
    {
        if (!Window::init(newSize, newResizable, newFullscreen, newTitle, newHighDpi, depth))
        {
            return false;
        }

        NSScreen* screen = [NSScreen mainScreen];

        CGSize windowSize;

        if (highDpi)
        {
            windowSize.width = size.v[0];
            windowSize.height = size.v[1];
        }
        else
        {
            windowSize.width = round(size.v[0] / screen.backingScaleFactor);
            windowSize.height = round(size.v[1] / screen.backingScaleFactor);
        }

        if (windowSize.width <= 0.0f) windowSize.width = round(screen.frame.size.width * 0.6);
        if (windowSize.height <= 0.0f) windowSize.height = round(screen.frame.size.height * 0.6);

        NSRect frame = NSMakeRect(round(screen.frame.size.width / 2.0f - windowSize.width / 2.0f),
                                  round(screen.frame.size.height / 2.0f - windowSize.height / 2.0f),
                                  windowSize.width, windowSize.height);

        NSUInteger windowStyleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;

        if (resizable)
        {
            windowStyleMask |= NSResizableWindowMask;
        }

        window  = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:windowStyleMask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO
                                                 screen:screen];
        [window setReleasedWhenClosed:NO];

        window.acceptsMouseMovedEvents = YES;
        windowDelegate = [[WindowDelegate alloc] initWithWindow:this];
        window.delegate = windowDelegate;

        [window setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary];

        if (fullscreen)
        {
            [window toggleFullScreen:nil];
        }

        [window setTitle:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:title.c_str()])];

        NSRect windowFrame = [NSWindow contentRectForFrameRect:[window frame]
                                                     styleMask:[window styleMask]];

        switch (sharedEngine->getRenderer()->getDriver())
        {
            case graphics::Renderer::Driver::EMPTY:
                view = [[NSView alloc] initWithFrame:windowFrame];
                break;
#if OUZEL_SUPPORTS_OPENGL
            case graphics::Renderer::Driver::OPENGL:
                view = [[OpenGLView alloc] initWithFrame:windowFrame];
                [view setWantsBestResolutionOpenGLSurface:highDpi ? YES : NO];
                break;
#endif
#if OUZEL_SUPPORTS_METAL
            case graphics::Renderer::Driver::METAL:
                view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
#endif
            default:
                Log(Log::Level::ERR) << "Unsupported render driver";
                return false;
        }

        window.contentView = view;
        [window makeKeyAndOrderFront:Nil];

        if (highDpi)
        {
            contentScale = static_cast<float>(window.backingScaleFactor);

            size.v[0] = static_cast<float>(windowSize.width);
            size.v[1] = static_cast<float>(windowSize.height);
        }
        else
        {
            size.v[0] = static_cast<float>(windowSize.width * window.backingScaleFactor);
            size.v[1] = static_cast<float>(windowSize.height * window.backingScaleFactor);
        }

        NSMenu* mainMenu = [[[NSMenu alloc] initWithTitle:@"Main Menu"] autorelease];

        NSMenuItem* mainMenuItem = [[[NSMenuItem alloc] initWithTitle:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:title.c_str()]) action:nil keyEquivalent:@""] autorelease];
        [mainMenu addItem:mainMenuItem];

        NSMenu* subMenu = [[[NSMenu alloc] initWithTitle:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:title.c_str()])] autorelease];
        [mainMenuItem setSubmenu:subMenu];

        NSMenuItem* quitItem = [[[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(handleQuit:) keyEquivalent:@"q"] autorelease];
        [subMenu addItem:quitItem];

        [NSApplication sharedApplication].mainMenu = mainMenu;

        return true;
    }

    void WindowMacOS::close()
    {
        Window::close();

        ouzel::sharedApplication->execute([this] {
            if (view)
            {
                [view release];
                view = Nil;
            }

            if (window)
            {
                window.delegate = Nil;
                [window close];
                [window release];
                window = Nil;
            }

            if (windowDelegate)
            {
                [windowDelegate release];
                windowDelegate = Nil;
            }
        });
    }

    void WindowMacOS::setSize(const Size2& newSize)
    {
        ouzel::sharedApplication->execute([this, newSize] {
            NSRect frame = [window frame];

            NSRect newFrame = [NSWindow frameRectForContentRect:
                               NSMakeRect(NSMinX(frame), NSMinY(frame), newSize.v[0], newSize.v[1])
                                                      styleMask:[window styleMask]];

            if (frame.size.width != newFrame.size.width ||
                frame.size.height != newFrame.size.height)
            {
                [window setFrame:newFrame display:YES animate:NO];
            }
        });

        Window::setSize(newSize);
    }

    void WindowMacOS::setFullscreen(bool newFullscreen)
    {
        if (fullscreen != newFullscreen)
        {
            ouzel::sharedApplication->execute([this, newFullscreen] {
                NSApplicationPresentationOptions options = [[NSApplication sharedApplication] presentationOptions];
                bool isFullscreen = (options & NSApplicationPresentationFullScreen) > 0;

                if (isFullscreen != newFullscreen)
                {
                    [window toggleFullScreen:nil];
                }
            });
        }

        Window::setFullscreen(newFullscreen);
    }

    void WindowMacOS::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            ouzel::sharedApplication->execute([this, newTitle] {
                NSString* objCTitle = [NSString stringWithCString:newTitle.c_str() encoding:NSUTF8StringEncoding];

                window.title = objCTitle;
            });
        }

        Window::setTitle(newTitle);
    }

    void WindowMacOS::handleResize()
    {
        NSRect frame = [NSWindow contentRectForFrameRect:window.frame
                                               styleMask:window.styleMask];

        Event event;
        event.type = Event::Type::WINDOW_SIZE_CHANGE;

        event.windowEvent.window = this;

        if (highDpi)
        {
            event.windowEvent.size = Size2(static_cast<float>(frame.size.width),
                                           static_cast<float>(frame.size.height));
        }
        else
        {
            event.windowEvent.size = Size2(static_cast<float>(frame.size.width * window.backingScaleFactor),
                                           static_cast<float>(frame.size.height * window.backingScaleFactor));
        }

        sharedEngine->getEventDispatcher()->postEvent(event);
    }

    void WindowMacOS::handleClose()
    {
        sharedEngine->stop();
    }

    void WindowMacOS::handleFullscreenChange(bool newFullscreen)
    {
        Event event;
        event.type = Event::Type::WINDOW_FULLSCREEN_CHANGE;

        event.windowEvent.window = this;
        event.windowEvent.fullscreen = newFullscreen;

        sharedEngine->getEventDispatcher()->postEvent(event);
    }

    void WindowMacOS::handleScaleFactorChange()
    {
        if (highDpi)
        {
            Event event;
            event.type = Event::Type::WINDOW_CONTENT_SCALE_CHANGE;

            event.windowEvent.window = this;
            event.windowEvent.contentScale = static_cast<float>(window.backingScaleFactor);

            sharedEngine->getEventDispatcher()->postEvent(event);
        }
        else
        {
            NSRect frame = [NSWindow contentRectForFrameRect:window.frame
                                                   styleMask:window.styleMask];

            Event event;
            event.type = Event::Type::WINDOW_SIZE_CHANGE;

            event.windowEvent.window = this;
            event.windowEvent.size = Size2(static_cast<float>(frame.size.width * window.backingScaleFactor),
                                           static_cast<float>(frame.size.height * window.backingScaleFactor));

            sharedEngine->getEventDispatcher()->postEvent(event);
        }
    }

    void WindowMacOS::handleScreenChange()
    {
        Event event;
        event.type = Event::Type::WINDOW_SCREEN_CHANGE;

        event.windowEvent.window = this;
        event.windowEvent.screenId = [[[[window screen] deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];

        sharedEngine->getEventDispatcher()->postEvent(event);
    }
}
