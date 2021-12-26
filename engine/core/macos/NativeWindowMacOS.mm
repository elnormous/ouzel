// Ouzel by Elviss Strazdins

#include <stdexcept>
#include "NativeWindowMacOS.hpp"
#include "ViewMacOS.h"
#include "../../platform/coregraphics/CoreGraphicsErrorCategory.hpp"
#include "../../graphics/RenderDevice.hpp"
#include "../../graphics/opengl/macos/OpenGLView.h"
#include "../../graphics/metal/macos/MetalView.h"

@interface WindowDelegate: NSObject<NSWindowDelegate>
@end

@implementation WindowDelegate
{
    ouzel::core::macos::NativeWindow* window;
}

- (id)initWithWindow:(ouzel::core::macos::NativeWindow*)initWindow
{
    if (self = [super init])
        window = initWindow;

    return self;
}

- (void)windowDidResize:(__unused NSNotification*)notification
{
    window->handleResize();
}

- (void)windowWillClose:(__unused NSNotification*)notification
{
    window->handleClose();
}

- (void)windowDidMiniaturize:(__unused NSNotification*)notification
{
    window->handleMinituarize();
}

- (void)windowDidDeminiaturize:(__unused NSNotification*)notification
{
    window->handleDeminituarize();
}

- (void)windowDidEnterFullScreen:(__unused NSNotification*)notification
{
    window->handleFullscreenChange(true);
}

- (void)windowDidExitFullScreen:(__unused NSNotification*)notification
{
    window->handleFullscreenChange(false);
}

- (void)windowDidChangeBackingProperties:(__unused NSNotification*)notification
{
    window->handleScaleFactorChange();
}

- (void)windowDidChangeScreen:(__unused NSNotification*)notification
{
    window->handleScreenChange();
}

- (void)windowDidBecomeKey:(__unused NSNotification*)notification
{
    window->handleBecomeKeyChange();
}

- (void)windowDidResignKey:(__unused NSNotification*)notification
{
    window->handleResignKeyChange();
}
@end

namespace ouzel::core::macos
{
    namespace
    {
        NSWindowStyleMask getWindowStyleMask(bool resiable,
                                             bool fullscreen,
                                             bool exclusiveFullscreen) noexcept
        {
            if (fullscreen && exclusiveFullscreen)
                return NSWindowStyleMaskBorderless;
            else
                return NSWindowStyleMaskTitled |
                    NSWindowStyleMaskClosable |
                    NSWindowStyleMaskMiniaturizable |
                    (resiable ? NSWindowStyleMaskResizable : 0);
        }
    }

    NativeWindow::NativeWindow(const math::Size<std::uint32_t, 2>& newSize,
                               bool newResizable,
                               bool newFullscreen,
                               bool newExclusiveFullscreen,
                               const std::string& newTitle,
                               graphics::Driver graphicsDriver,
                               bool newHighDpi):
        core::NativeWindow{
            newSize,
            newResizable,
            newFullscreen,
            newExclusiveFullscreen,
            newTitle,
            newHighDpi
        }
    {
        screen = [NSScreen mainScreen];
        displayId = [[[screen deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];

        CGSize windowSize = CGSizeMake(size.v[0], size.v[1]);
        if (windowSize.width <= 0.0F) windowSize.width = std::round(screen.frame.size.width * 0.8);
        if (windowSize.height <= 0.0F) windowSize.height = std::round(screen.frame.size.height * 0.8);

        const NSRect frame = NSMakeRect(std::round(screen.frame.size.width / 2.0F - windowSize.width / 2.0F),
                                        std::round(screen.frame.size.height / 2.0F - windowSize.height / 2.0F),
                                        windowSize.width, windowSize.height);

        window = [[NSWindow alloc] initWithContentRect:frame
                                             styleMask:getWindowStyleMask(resizable,
                                                                          fullscreen,
                                                                          exclusiveFullscreen)
                                               backing:NSBackingStoreBuffered
                                                 defer:NO
                                                screen:screen];
        
        [window setFrameAutosaveName:@"Main Window"];

        const NSRect realFrame = [NSWindow contentRectForFrameRect:window.frame
                                                         styleMask:window.styleMask];
        windowSize.width = static_cast<std::uint32_t>(realFrame.size.width);
        windowSize.height = static_cast<std::uint32_t>(realFrame.size.height);

        [window setReleasedWhenClosed:NO];

        window.acceptsMouseMovedEvents = YES;
        windowDelegate = [[WindowDelegate alloc] initWithWindow:this];
        window.delegate = windowDelegate;

        [window setCollectionBehavior:exclusiveFullscreen ? NSWindowCollectionBehaviorFullScreenAuxiliary : NSWindowCollectionBehaviorFullScreenPrimary];

        if (fullscreen)
        {
            if (exclusiveFullscreen)
            {
                if (const auto result = CGDisplayCapture(displayId); result != kCGErrorSuccess)
                    throw std::system_error{result, platform::coregraphics::getErrorCategory(), "Failed to capture the main display"};

                windowRect = frame;

                const NSRect screenRect = [screen frame];
                [window setFrame:screenRect display:YES animate:NO];

                const CGWindowLevel windowLevel = CGShieldingWindowLevel();
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
            [invocation setSelector:@selector(setAllowsAutomaticWindowTabbing:)];
            [invocation setArgument:&value atIndex:2];
            [invocation invoke];
        }

        const NSRect windowFrame = [NSWindow contentRectForFrameRect:[window frame]
                                                           styleMask:[window styleMask]];

        switch (graphicsDriver)
        {
            case graphics::Driver::empty:
                view = [[View alloc] initWithFrame:windowFrame];
                break;
#if OUZEL_COMPILE_OPENGL
            case graphics::Driver::openGl:
                view = [[OpenGLView alloc] initWithFrame:windowFrame];
                [view setWantsBestResolutionOpenGLSurface:highDpi ? YES : NO];
                break;
#endif
#if OUZEL_COMPILE_METAL
            case graphics::Driver::metal:
                view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
#endif
            default:
                throw std::runtime_error{"Unsupported render driver"};
        }

        [view setAcceptsTouchEvents:YES];
        [view setWantsRestingTouches:YES];

        window.contentView = view;
        [window makeKeyAndOrderFront:nil];

        size.v[0] = static_cast<std::uint32_t>(windowSize.width);
        size.v[1] = static_cast<std::uint32_t>(windowSize.height);

        if (highDpi)
        {
            contentScale = static_cast<float>(screen.backingScaleFactor);
            resolution = size * static_cast<std::uint32_t>(contentScale);
        }
        else
        {
            contentScale = 1.0F;
            resolution = size;
        }
    }

    NativeWindow::~NativeWindow()
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

    void NativeWindow::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::changeSize:
                setSize(command.size);
                break;
            case Command::Type::changeFullscreen:
                setFullscreen(command.fullscreen);
                break;
            case Command::Type::close:
                close();
                break;
            case Command::Type::setTitle:
                setTitle(command.title);
                break;
            case Command::Type::bringToFront:
                bringToFront();
                break;
            case Command::Type::show:
                show();
                break;
            case Command::Type::hide:
                hide();
                break;
            case Command::Type::minimize:
                minimize();
                break;
            case Command::Type::maximize:
                maximize();
                break;
            case Command::Type::restore:
                restore();
                break;
            default:
                throw std::runtime_error{"Invalid command"};
        }
    }

    void NativeWindow::close()
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

    void NativeWindow::setSize(const math::Size<std::uint32_t, 2>& newSize)
    {
        size = newSize;

        const NSRect frame = [window frame];
        const NSRect contentRect = NSMakeRect(NSMinX(frame), NSMinY(frame), newSize.v[0], newSize.v[1]);

        const NSRect newFrame = [NSWindow frameRectForContentRect:contentRect
                                                        styleMask:[window styleMask]];

        if (frame.size.width != newFrame.size.width ||
            frame.size.height != newFrame.size.height)
        {
            [window setFrame:newFrame display:YES animate:NO];
            resolution = size * static_cast<std::uint32_t>(contentScale);

            Event resolutionChangeEvent{Event::Type::resolutionChange};
            resolutionChangeEvent.size = resolution;
            sendEvent(resolutionChangeEvent);
        }
    }

    void NativeWindow::setFullscreen(bool newFullscreen)
    {
        if (fullscreen != newFullscreen)
        {
            [window setStyleMask:getWindowStyleMask(resizable,
                                                    fullscreen,
                                                    exclusiveFullscreen)];

            if (exclusiveFullscreen)
            {
                if (newFullscreen)
                {
                    if (const auto result = CGDisplayCapture(displayId); result != kCGErrorSuccess)
                        throw std::system_error{result, platform::coregraphics::getErrorCategory(), "Failed to capture the main display"};

                    windowRect = [window frame];

                    const NSRect screenRect = [screen frame];
                    [window setFrame:screenRect display:YES animate:NO];

                    const CGWindowLevel windowLevel = CGShieldingWindowLevel();
                    [window setLevel:windowLevel];
                    [window makeKeyAndOrderFront:nil];
                }
                else
                {
                    [window setFrame:windowRect display:YES animate:NO];

                    if (const auto result = CGDisplayRelease(displayId); result != kCGErrorSuccess)
                        throw std::system_error{result, platform::coregraphics::getErrorCategory(), "Failed to release the main display"};
                }
            }
            else
            {
                const NSApplicationPresentationOptions options = [NSApp presentationOptions];
                const bool isFullscreen = (options & NSApplicationPresentationFullScreen) > 0;

                if (isFullscreen != newFullscreen)
                    [window toggleFullScreen:nil];
            }
        }

        fullscreen = newFullscreen;
    }

    void NativeWindow::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            NSString* objCTitle = [NSString stringWithCString:newTitle.c_str() encoding:NSUTF8StringEncoding];

            window.title = objCTitle;
            title = newTitle;
        }
    }

    void NativeWindow::bringToFront()
    {
        [window orderFront:nil];
    }

    void NativeWindow::show()
    {
        [window orderFront:nil];
    }

    void NativeWindow::hide()
    {
        [window orderOut:nil];
    }

    void NativeWindow::minimize()
    {
        [window miniaturize:nil];
    }

    void NativeWindow::maximize()
    {
        windowRect = [screen visibleFrame];
        [window setFrame:windowRect display:YES];
    }

    void NativeWindow::restore()
    {
        [window deminiaturize:nil];
    }

    void NativeWindow::handleResize()
    {
        const NSRect frame = [NSWindow contentRectForFrameRect:window.frame
                                                     styleMask:window.styleMask];

        size = math::Size<std::uint32_t, 2>{
            static_cast<std::uint32_t>(frame.size.width),
            static_cast<std::uint32_t>(frame.size.height)
        };
        resolution = size * static_cast<std::uint32_t>(contentScale);

        Event sizeChangeEvent{Event::Type::sizeChange};
        sizeChangeEvent.size = size;
        sendEvent(sizeChangeEvent);

        Event resolutionChangeEvent{Event::Type::resolutionChange};
        resolutionChangeEvent.size = resolution;
        sendEvent(resolutionChangeEvent);
    }

    void NativeWindow::handleClose()
    {
        sendEvent(Event(Event::Type::close));
    }

    void NativeWindow::handleMinituarize()
    {
        sendEvent(Event(Event::Type::minimize));
    }

    void NativeWindow::handleDeminituarize()
    {
        sendEvent(Event(Event::Type::restore));
    }

    void NativeWindow::handleFullscreenChange(bool newFullscreen)
    {
        fullscreen = newFullscreen;

        Event fullscreenChangeEvent{Event::Type::fullscreenChange};
        fullscreenChangeEvent.fullscreen = fullscreen;
        sendEvent(fullscreenChangeEvent);
    }

    void NativeWindow::handleScaleFactorChange()
    {
        if (highDpi)
        {
            contentScale = static_cast<float>(window.backingScaleFactor);
            resolution = size * static_cast<std::uint32_t>(contentScale);

            Event resolutionChangeEvent{Event::Type::resolutionChange};
            resolutionChangeEvent.size = resolution;
            sendEvent(resolutionChangeEvent);
        }
    }

    void NativeWindow::handleScreenChange()
    {
        screen = [window screen];
        displayId = [[[screen deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];

        Event screenChangeEvent{Event::Type::screenChange};
        screenChangeEvent.displayId = displayId;
        sendEvent(screenChangeEvent);
    }

    void NativeWindow::handleBecomeKeyChange()
    {
        Event focusChangeEvent{Event::Type::focusChange};
        focusChangeEvent.focus = true;
        sendEvent(focusChangeEvent);
    }

    void NativeWindow::handleResignKeyChange()
    {
        Event focusChangeEvent{Event::Type::focusChange};
        focusChangeEvent.focus = false;
        sendEvent(focusChangeEvent);
    }
}
