// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <CoreGraphics/CGGeometry.h>

#if defined(__OBJC__)
#import <Cocoa/Cocoa.h>
#import <CoreGraphics/CoreGraphics.h>
typedef NSWindow* NSWindowPtr;
typedef NSView* NSViewPtr;
typedef id<NSWindowDelegate> NSWindowDelegatePtr;
typedef NSScreen* NSScreenPtr;
#else
#include <objc/NSObjCRuntime.h>
typedef id NSWindowPtr;
typedef id NSViewPtr;
typedef id NSWindowDelegatePtr;
typedef id NSScreenPtr;
typedef uint32_t CGDirectDisplayID;
#endif

#include "core/NativeWindow.hpp"
#include "graphics/Renderer.hpp"

namespace ouzel
{
    class Window;

    class NativeWindowMacOS: public NativeWindow
    {
        friend Window;
    public:
        virtual ~NativeWindowMacOS();

        virtual void close() override;

        virtual void setSize(const Size2& newSize) override;
        virtual void setFullscreen(bool newFullscreen) override;
        virtual void setTitle(const std::string& newTitle) override;

        void handleResize();
        void handleClose();
        void handleFullscreenChange(bool newFullscreen);
        void handleScaleFactorChange();
        void handleScreenChange();

        inline NSWindowPtr getNativeWindow() const { return window; }
        inline NSViewPtr getNativeView() const { return view; }
        inline NSScreenPtr getScreen() const { return screen; }
        inline CGDirectDisplayID getDisplayId() const { return displayId; }

    protected:
        NativeWindowMacOS(EventHandler& initEventHandler,
                          const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          graphics::Renderer::Driver graphicsDriver,
                          bool newHighDpi);

        NSWindowPtr window = nil;
        NSViewPtr view = nil;
        NSWindowDelegatePtr windowDelegate = nil;
        NSScreenPtr screen = nil;
        CGDirectDisplayID displayId = 0;
        NSUInteger windowStyleMask = 0;
        CGRect windowRect;
    };
}
