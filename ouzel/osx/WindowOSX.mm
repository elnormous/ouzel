// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowOSX.h"
#include "OpenGLView.h"
#include "MetalView.h"
#include "Utils.h"

@interface WindowDelegate: NSObject<NSWindowDelegate>
{
    ouzel::WindowOSX* _window;
}

@end

@implementation WindowDelegate

-(void)handleQuit:(id)sender
{
    OUZEL_UNUSED(sender);

    if ([NSThread isMainThread])
    {
        [_window->getNativeWindow() close];
    }
    else
    {
        dispatch_async(dispatch_get_main_queue(), ^{
            [_window->getNativeWindow() close];
        });
    }
}

-(id)initWithWindow:(ouzel::WindowOSX*)window
{
    if (self = [super init])
    {
        _window = window;
    }

    return self;
}

-(void)windowDidResize:(NSNotification *)notification
{
    OUZEL_UNUSED(notification);
    _window->handleResize();
}

-(void)windowDidChangeScreen:(NSNotification *)notification
{
    OUZEL_UNUSED(notification);
    _window->handleDisplayChange();
}

-(void)windowWillClose:(NSNotification *)notification
{
    OUZEL_UNUSED(notification);
    _window->handleClose();
}

-(void)windowDidEnterFullScreen:(NSNotification *)notification
{
    OUZEL_UNUSED(notification);
    _window->handleFullscreenChange(true);
}

-(void)windowDidExitFullScreen:(NSNotification *)notification
{
    OUZEL_UNUSED(notification);
    _window->handleFullscreenChange(false);
}

@end

namespace ouzel
{
    WindowOSX::WindowOSX(const Size2& size, bool resizable, bool fullscreen, const std::string& title, graphics::Renderer::Driver driver):
        Window(size, resizable, fullscreen, title, driver)
    {

    }

    WindowOSX::~WindowOSX()
    {
        close();
    }

    bool WindowOSX::init()
    {
        NSScreen* screen = [NSScreen mainScreen];
        NSRect screenFrame = screen.frame;

        NSRect frame = NSMakeRect(screenFrame.size.width / 2 - size.width / 2,
                                  screenFrame.size.height / 2 - size.height / 2,
                                  size.width, size.height);

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

        windowDelegate = [[WindowDelegate alloc] initWithWindow: this];
        window.delegate = windowDelegate;

        [window setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary];

        if (fullscreen)
        {
            [window toggleFullScreen:nil];
        }

        [window setTitle:[NSString stringWithUTF8String:title.c_str()]];

        NSRect windowFrame = [NSWindow contentRectForFrameRect:[window frame]
                                                     styleMask:[window styleMask]];

        switch (driver)
        {
            case graphics::Renderer::Driver::OPENGL:
                view = [[OpenGLView alloc] initWithFrame:windowFrame];
                break;
            case graphics::Renderer::Driver::METAL:
                view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
            default:
                log("Unsupported render driver");
                return false;
        }

        window.contentView = view;
        [window makeKeyAndOrderFront:Nil];

        NSMenu* mainMenu = [[[NSMenu alloc] initWithTitle:@"Main Menu"] autorelease];

        NSMenuItem* mainMenuItem = [[[NSMenuItem alloc] initWithTitle:@"Ouzel" action:nil keyEquivalent:@""] autorelease];
        [mainMenu addItem:mainMenuItem];

        NSMenu* subMenu = [[[NSMenu alloc] initWithTitle:@"Ouzel"] autorelease];
        [mainMenuItem setSubmenu:subMenu];

        NSMenuItem* quitItem = [[[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(handleQuit:) keyEquivalent:@"q"] autorelease];
        [subMenu addItem:quitItem];

        [NSApplication sharedApplication].mainMenu = mainMenu;

        return Window::init();
    }

    void WindowOSX::close()
    {
        if (view)
        {
            [view close];
            [view release];
            view = Nil;
        }

        if (windowDelegate)
        {
            [windowDelegate release];
            windowDelegate = Nil;
        }

        if (window)
        {
            window.delegate = Nil;
            [window release];
            window = Nil;
        }
    }

    void WindowOSX::setSize(const Size2& newSize)
    {
        NSRect frame = [NSWindow contentRectForFrameRect:[window frame]
                                               styleMask:[window styleMask]];

        NSRect newFrame = [NSWindow frameRectForContentRect:
                           NSMakeRect(NSMinX(frame), NSMaxY(frame) - newSize.height, newSize.width, newSize.height)
                                                  styleMask:[window styleMask]];

        if (frame.size.width != newFrame.size.width ||
            frame.size.height != newFrame.size.height)
        {
            if ([NSThread isMainThread])
            {
                [window setFrame:newFrame display:YES animate:NO];
            }
            else
            {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [window setFrame:newFrame display:YES animate:NO];
                });
            }
        }

        Window::setSize(newSize);
    }

    void WindowOSX::setFullscreen(bool newFullscreen)
    {
        if (fullscreen != newFullscreen)
        {
            if ([NSThread isMainThread])
            {
                [window toggleFullScreen:nil];
            }
            else
            {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [window toggleFullScreen:nil];
                });
            }
        }

        Window::setFullscreen(fullscreen);
    }

    void WindowOSX::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            NSString* objCTitle = [NSString stringWithCString:newTitle.c_str() encoding:NSUTF8StringEncoding];

            if ([NSThread isMainThread])
            {
                window.title = objCTitle;
            }
            else
            {
                dispatch_async(dispatch_get_main_queue(), ^{
                    window.title = objCTitle;
                });
            }
        }

        Window::setTitle(newTitle);
    }

    void WindowOSX::handleResize()
    {
        NSRect frame = [NSWindow contentRectForFrameRect:[window frame]
                                               styleMask:[window styleMask]];

        Window::setSize(Size2(static_cast<float>(frame.size.width),
                              static_cast<float>(frame.size.height)));
    }

    void WindowOSX::handleDisplayChange()
    {
        [view changeDisplay];
    }

    void WindowOSX::handleClose()
    {
        close();
    }

    void WindowOSX::handleFullscreenChange(bool fullscreen)
    {
        Window::setFullscreen(fullscreen);
    }
}
