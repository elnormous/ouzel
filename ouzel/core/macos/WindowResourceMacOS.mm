// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowResourceMacOS.hpp"
#include "ViewMacOS.h"
#include "graphics/RenderDevice.hpp"
#include "graphics/opengl/macos/OpenGLView.h"
#include "graphics/metal/macos/MetalView.h"
#include "input/macos/InputManagerMacOS.hpp"
#include "core/Engine.hpp"
#include "thread/Lock.hpp"
#include "utils/Log.hpp"

@interface WindowDelegate: NSObject<NSWindowDelegate>
{
    ouzel::WindowResourceMacOS* window;
}

@end

@implementation WindowDelegate

-(id)initWithWindow:(ouzel::WindowResourceMacOS*)initWindow
{
    if (self = [super init])
    {
        window = initWindow;
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
    WindowResourceMacOS::WindowResourceMacOS()
    {
    }

    WindowResourceMacOS::~WindowResourceMacOS()
    {
        if (exclusiveFullscreen && fullscreen)
        {
            if (CGDisplayRelease(displayId) != kCGErrorSuccess)
            {
                Log(Log::Level::ERR) << "Failed to release the main display";
            }
        }

        if (view)
        {
            [view release];
        }

        if (window)
        {
            window.delegate = nil;
            [window close];
            [window release];
        }

        if (windowDelegate)
        {
            [windowDelegate release];
        }
    }

    bool WindowResourceMacOS::init(const Size2& newSize,
                                   bool newResizable,
                                   bool newFullscreen,
                                   bool newExclusiveFullscreen,
                                   const std::string& newTitle,
                                   bool newHighDpi,
                                   bool depth)
    {
        if (!WindowResource::init(newSize,
                                  newResizable,
                                  newFullscreen,
                                  newExclusiveFullscreen,
                                  newTitle,
                                  newHighDpi,
                                  depth))
        {
            return false;
        }

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

        [window setCollectionBehavior:exclusiveFullscreen ? NSWindowCollectionBehaviorFullScreenAuxiliary : NSWindowCollectionBehaviorFullScreenPrimary];

        if (exclusiveFullscreen)
        {
            if (fullscreen)
            {
                if (CGDisplayCapture(displayId) != kCGErrorSuccess)
                {
                    Log(Log::Level::ERR) << "Failed to capture the main display";
                }

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
            {
                [window toggleFullScreen:nil];
            }
        }

        [window setTitle:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:title.c_str()])];

        NSRect windowFrame = [NSWindow contentRectForFrameRect:[window frame]
                                                     styleMask:[window styleMask]];

        switch (engine->getRenderer()->getDevice()->getDriver())
        {
            case graphics::Renderer::Driver::EMPTY:
                view = [[ViewMacOS alloc] initWithFrame:windowFrame];
                break;
#if OUZEL_COMPILE_OPENGL
            case graphics::Renderer::Driver::OPENGL:
                view = [[OpenGLView alloc] initWithFrame:windowFrame];
                [view setWantsBestResolutionOpenGLSurface:highDpi ? YES : NO];
                break;
#endif
#if OUZEL_COMPILE_METAL
            case graphics::Renderer::Driver::METAL:
                view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
#endif
            default:
                Log(Log::Level::ERR) << "Unsupported render driver";
                return false;
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

        return true;
    }

    void WindowResourceMacOS::close()
    {
        WindowResource::close();

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

    void WindowResourceMacOS::setSize(const Size2& newSize)
    {
        WindowResource::setSize(newSize);

        NSRect frame = [window frame];

        NSRect newFrame = [NSWindow frameRectForContentRect:NSMakeRect(NSMinX(frame), NSMinY(frame), newSize.width, newSize.height)
                                                  styleMask:[window styleMask]];

        if (frame.size.width != newFrame.size.width ||
            frame.size.height != newFrame.size.height)
        {
            [window setFrame:newFrame display:YES animate:NO];
            resolution = size * contentScale;

            Lock lock(listenerMutex);
            if (listener) listener->onResolutionChange(resolution);
        }
    }

    void WindowResourceMacOS::setFullscreen(bool newFullscreen)
    {
        if (fullscreen != newFullscreen)
        {
            if (exclusiveFullscreen)
            {
                if (newFullscreen)
                {
                    if (CGDisplayCapture(displayId) != kCGErrorSuccess)
                    {
                        Log(Log::Level::ERR) << "Failed to capture the main display";
                    }

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
                    {
                        Log(Log::Level::ERR) << "Failed to release the main display";
                    }
                }
            }
            else
            {
                NSApplicationPresentationOptions options = [[NSApplication sharedApplication] presentationOptions];
                bool isFullscreen = (options & NSApplicationPresentationFullScreen) > 0;

                if (isFullscreen != newFullscreen)
                {
                    [window toggleFullScreen:nil];
                }
            }
        }

        WindowResource::setFullscreen(newFullscreen);
    }

    void WindowResourceMacOS::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            NSString* objCTitle = [NSString stringWithCString:newTitle.c_str() encoding:NSUTF8StringEncoding];

            window.title = objCTitle;
        }

        WindowResource::setTitle(newTitle);
    }

    void WindowResourceMacOS::handleResize()
    {
        NSRect frame = [NSWindow contentRectForFrameRect:window.frame
                                               styleMask:window.styleMask];

        size = Size2(static_cast<float>(frame.size.width),
                     static_cast<float>(frame.size.height));
        resolution = size * contentScale;

        Lock lock(listenerMutex);
        if (listener)
        {
            listener->onSizeChange(size);
            listener->onResolutionChange(resolution);
        }
    }

    void WindowResourceMacOS::handleClose()
    {
        Lock lock(listenerMutex);
        if (listener) listener->onClose();
    }

    void WindowResourceMacOS::handleFullscreenChange(bool newFullscreen)
    {
        fullscreen = newFullscreen;

        Lock lock(listenerMutex);
        if (listener) listener->onFullscreenChange(newFullscreen);
    }

    void WindowResourceMacOS::handleScaleFactorChange()
    {
        if (highDpi)
        {
            contentScale = static_cast<float>(window.backingScaleFactor);
            resolution = size * contentScale;

            Lock lock(listenerMutex);
            if (listener) listener->onResolutionChange(resolution);
        }
    }

    void WindowResourceMacOS::handleScreenChange()
    {
        screen = [window screen];
        displayId = [[[screen deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];

        Lock lock(listenerMutex);
        if (listener) listener->onScreenChange(displayId);
    }
}
