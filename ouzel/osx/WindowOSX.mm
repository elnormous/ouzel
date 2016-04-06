// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <Cocoa/Cocoa.h>
#include "WindowOSX.h"
#include "OpenGLView.h"
#include "Utils.h"

@interface WindowDelegate: NSObject <NSWindowDelegate>
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
    WindowOSX::WindowOSX(const Size2& size, bool resizable, bool fullscreen, const std::string& title, video::Renderer::Driver driver):
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

        NSRect frame = NSMakeRect(screenFrame.size.width / 2 - _size.width / 2,
                                  screenFrame.size.height / 2 - _size.height / 2,
                                  _size.width, _size.height);

        NSUInteger windowStyleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;

        if (_resizable)
        {
            windowStyleMask |= NSResizableWindowMask;
        }

        _window  = [[NSWindow alloc] initWithContentRect:frame
                                               styleMask:windowStyleMask
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO
                                                  screen:screen];
        [_window setReleasedWhenClosed:NO];

        _window.backgroundColor = [NSColor blackColor];
        _window.acceptsMouseMovedEvents = YES;

        WindowDelegate* windowDelegate = [[WindowDelegate alloc] initWithWindow: this];
        _window.delegate = windowDelegate;

        [_window setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary];

        if (_fullscreen)
        {
            [_window toggleFullScreen:nil];
        }

        [_window setTitle:[NSString stringWithUTF8String:_title.c_str()]];

        NSRect windowFrame = [NSWindow contentRectForFrameRect:[_window frame]
                                                     styleMask:[_window styleMask]];

        _view = [[OpenGLView alloc] initWithFrame:windowFrame];

        [_window setContentView:_view];

        [_window makeKeyAndOrderFront:Nil];

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
        if (_view)
        {
            [_view close];
            [_view release];
            _view = Nil;
        }

        if (_window)
        {
            [_window.delegate release];
            [_window release];
            _window = Nil;
        }
    }

    void WindowOSX::setSize(const Size2& size)
    {
        NSRect frame = [NSWindow contentRectForFrameRect:[_window frame]
                                               styleMask:[_window styleMask]];

        NSRect newFrame = [NSWindow frameRectForContentRect:
                           NSMakeRect(NSMinX(frame), NSMaxY(frame) - _size.height, _size.width, _size.height)
                                                  styleMask:[_window styleMask]];

        if (frame.size.width != newFrame.size.width ||
            frame.size.height != newFrame.size.height)
        {
            if ([NSThread isMainThread])
            {
                [_window setFrame:newFrame display:YES animate:NO];
            }
            else
            {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [_window setFrame:newFrame display:YES animate:NO];
                });
            }
        }

        Window::setSize(size);
    }

    void WindowOSX::setFullscreen(bool fullscreen)
    {
        if (_fullscreen != fullscreen)
        {
            if ([NSThread isMainThread])
            {
                [_window toggleFullScreen:nil];
            }
            else
            {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [_window toggleFullScreen:nil];
                });
            }
        }

        Window::setFullscreen(fullscreen);
    }

    void WindowOSX::setTitle(const std::string& title)
    {
        if (_title != title)
        {
            NSString* objCTitle = [NSString stringWithCString:title.c_str() encoding:NSUTF8StringEncoding];

            if ([NSThread isMainThread])
            {
                _window.title = objCTitle;
            }
            else
            {
                dispatch_async(dispatch_get_main_queue(), ^{
                    _window.title = objCTitle;
                });
            }
        }

        Window::setTitle(title);
    }

    void WindowOSX::handleResize()
    {
        NSRect frame = [NSWindow contentRectForFrameRect:[_window frame]
                                               styleMask:[_window styleMask]];

        Window::setSize(Size2(static_cast<float>(frame.size.width),
                              static_cast<float>(frame.size.height)));
    }

    void WindowOSX::handleDisplayChange()
    {
        [_view changeDisplay];
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
