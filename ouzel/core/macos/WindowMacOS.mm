// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowMacOS.h"
#include "graphics/macos/OpenGLView.h"
#include "graphics/macos/MetalView.h"
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

@end

namespace ouzel
{
    WindowMacOS::WindowMacOS(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle):
        Window(aSize, aResizable, aFullscreen, aTitle)
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

    bool WindowMacOS::init()
    {
        NSScreen* screen = [NSScreen mainScreen];

        if (size.width <= 0.0f) size.width = static_cast<float>(screen.frame.size.width) * 0.6f;
        if (size.height <= 0.0f) size.height = static_cast<float>(screen.frame.size.height) * 0.6f;

        NSRect frame = NSMakeRect(static_cast<float>(screen.frame.size.width) / 2.0f - size.width / 2.0f,
                                  static_cast<float>(screen.frame.size.height) / 2.0f - size.height / 2.0f,
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
            case graphics::Renderer::Driver::OPENGL:
                view = [[OpenGLView alloc] initWithFrame:windowFrame];
                break;
            case graphics::Renderer::Driver::METAL:
                view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
            default:
                Log(Log::Level::ERR) << "Unsupported render driver";
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

    void WindowMacOS::close()
    {
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
    }

    void WindowMacOS::setSize(const Size2& newSize)
    {
        ouzel::sharedApplication->execute([this, newSize] {
            NSRect frame = [window frame];

            NSRect newFrame = [NSWindow frameRectForContentRect:
                               NSMakeRect(NSMinX(frame), NSMinY(frame), newSize.width, newSize.height)
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
                NSApplicationPresentationOptions opts = [[NSApplication sharedApplication ] presentationOptions];
                bool isFullscreen = opts & NSApplicationPresentationFullScreen;

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

    float WindowMacOS::getContentScale() const
    {
        return static_cast<float>(window.backingScaleFactor);
    }

    void WindowMacOS::handleResize()
    {
        NSRect frame = [NSWindow contentRectForFrameRect:[window frame]
                                               styleMask:[window styleMask]];

        Window::setSize(Size2(static_cast<float>(frame.size.width),
                              static_cast<float>(frame.size.height)));
    }

    void WindowMacOS::handleClose()
    {
        close();
    }

    void WindowMacOS::handleFullscreenChange(bool fullscreen)
    {
        Window::setFullscreen(fullscreen);
    }
}
