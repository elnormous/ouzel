// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "NativeWindowMacOS.hpp"
#include "ViewMacOS.h"
#include "graphics/RenderDevice.hpp"
#include "graphics/opengl/macos/OpenGLView.h"
#include "graphics/metal/macos/MetalView.h"

@interface WindowDelegate: NSObject<NSWindowDelegate>

@end

@implementation WindowDelegate
{
    ouzel::NativeWindowMacOS* window;
}

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

-(void)windowDidBecomeKey:(__unused NSNotification*)notification
{
    window->handleBecomeKeyChange();
}

-(void)windowDidResignKey:(__unused NSNotification*)notification
{
    window->handleResignKeyChange();
}

@end

namespace ouzel
{
    NativeWindowMacOS::NativeWindowMacOS(const std::function<void(const Event&)>& initCallback,
                                         const Size<2, uint32_t>& newSize,
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
            windowSize.width = size.v[0];
            windowSize.height = size.v[1];
        }
        else
        {
            windowSize.width = round(size.v[0] / screen.backingScaleFactor);
            windowSize.height = round(size.v[1] / screen.backingScaleFactor);
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

        if (fullscreen)
        {
            if (exclusiveFullscreen)
            {
                if (CGDisplayCapture(displayId) != kCGErrorSuccess)
                    throw std::runtime_error("Failed to capture the main display");

                windowRect = frame;
                [window setStyleMask:NSBorderlessWindowMask];

                NSRect screenRect = [screen frame];
                [window setFrame:screenRect display:YES animate:NO];

                CGWindowLevel windowLevel = CGShieldingWindowLevel();
                [window setLevel:windowLevel];
            }
            else
                [window toggleFullScreen:nil];
        }

        [window setTitle:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:title.c_str()])];

        if ([NSWindow respondsToSelector:@selector(setAllowsAutomaticWindowTabbing:)])
        {
            BOOL value = NO;

            NSMethodSignature* signature = [NSWindow methodSignatureForSelector:@selector(setAllowsAutomaticWindowTabbing:)];
            NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:signature];
            [invocation setSelector: @selector(setAllowsAutomaticWindowTabbing:)];
            [invocation setArgument: &value atIndex: 2];
            [invocation invoke];
        }

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
                throw std::runtime_error("Unsupported render driver");
        }

        [view setAcceptsTouchEvents:YES];
        [view setWantsRestingTouches:YES];

        window.contentView = view;
        [window makeKeyAndOrderFront:nil];

        size.v[0] = static_cast<uint32_t>(windowSize.width);
        size.v[1] = static_cast<uint32_t>(windowSize.height);

        if (highDpi)
        {
            contentScale = static_cast<float>(screen.backingScaleFactor);
            resolution = size * static_cast<uint32_t>(contentScale);
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

    void NativeWindowMacOS::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::CHANGE_SIZE:
                setSize(command.size);
                break;
            case Command::Type::CHANGE_FULLSCREEN:
                setFullscreen(command.fullscreen);
                break;
            case Command::Type::CLOSE:
                close();
                break;
            case Command::Type::SET_TITLE:
                setTitle(command.title);
                break;
            default:
                throw std::runtime_error("Invalid command");
        }
    }

    void NativeWindowMacOS::close()
    {
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

    void NativeWindowMacOS::setSize(const Size<2, uint32_t>& newSize)
    {
        size = newSize;

        NSRect frame = [window frame];

        NSRect newFrame = [NSWindow frameRectForContentRect:NSMakeRect(NSMinX(frame), NSMinY(frame), newSize.v[0], newSize.v[1])
                                                  styleMask:[window styleMask]];

        if (frame.size.width != newFrame.size.width ||
            frame.size.height != newFrame.size.height)
        {
            [window setFrame:newFrame display:YES animate:NO];
            resolution = size * static_cast<uint32_t>(contentScale);

            Event resolutionChangeEvent(Event::Type::RESOLUTION_CHANGE);
            resolutionChangeEvent.resolution = resolution;
            sendEvent(resolutionChangeEvent);
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
                        throw std::runtime_error("Failed to capture the main display");

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
                        throw std::runtime_error("Failed to release the main display");
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

        fullscreen = newFullscreen;
    }

    void NativeWindowMacOS::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            NSString* objCTitle = [NSString stringWithCString:newTitle.c_str() encoding:NSUTF8StringEncoding];

            window.title = objCTitle;
        }

        title = newTitle;
    }

    void NativeWindowMacOS::handleResize()
    {
        NSRect frame = [NSWindow contentRectForFrameRect:window.frame
                                               styleMask:window.styleMask];

        size = Size<2, uint32_t>(static_cast<uint32_t>(frame.size.width),
                                 static_cast<uint32_t>(frame.size.height));
        resolution = size * static_cast<uint32_t>(contentScale);

        Event sizeChangeEvent(Event::Type::SIZE_CHANGE);
        sizeChangeEvent.size = size;
        sendEvent(sizeChangeEvent);

        Event resolutionChangeEvent(Event::Type::RESOLUTION_CHANGE);
        resolutionChangeEvent.resolution = resolution;
        sendEvent(resolutionChangeEvent);
    }

    void NativeWindowMacOS::handleClose()
    {
        sendEvent(Event(Event::Type::CLOSE));
    }

    void NativeWindowMacOS::handleFullscreenChange(bool newFullscreen)
    {
        fullscreen = newFullscreen;

        Event fullscreenChangeEvent(Event::Type::FULLSCREEN_CHANGE);
        fullscreenChangeEvent.fullscreen = fullscreen;
        sendEvent(fullscreenChangeEvent);
    }

    void NativeWindowMacOS::handleScaleFactorChange()
    {
        if (highDpi)
        {
            contentScale = static_cast<float>(window.backingScaleFactor);
            resolution = size * static_cast<uint32_t>(contentScale);

            Event resolutionChangeEvent(Event::Type::RESOLUTION_CHANGE);
            resolutionChangeEvent.resolution = resolution;
            sendEvent(resolutionChangeEvent);
        }
    }

    void NativeWindowMacOS::handleScreenChange()
    {
        screen = [window screen];
        displayId = [[[screen deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];

        Event screenChangeEvent(Event::Type::SCREEN_CHANGE);
        screenChangeEvent.displayId = displayId;
        sendEvent(screenChangeEvent);
    }

    void NativeWindowMacOS::handleBecomeKeyChange()
    {
        Event focusChangeEvent(Event::Type::FOCUS_CHANGE);
        focusChangeEvent.focus = true;
        sendEvent(focusChangeEvent);
    }

    void NativeWindowMacOS::handleResignKeyChange()
    {
        Event focusChangeEvent(Event::Type::FOCUS_CHANGE);
        focusChangeEvent.focus = false;
        sendEvent(focusChangeEvent);
    }
}
