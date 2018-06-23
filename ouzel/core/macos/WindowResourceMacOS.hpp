// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

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

#include "core/WindowResource.hpp"

namespace ouzel
{
    class Window;

    class WindowResourceMacOS: public WindowResource
    {
        friend Window;
    public:
        virtual ~WindowResourceMacOS();

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
        WindowResourceMacOS();
        virtual void init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth) override;

        NSWindowPtr window = nil;
        NSViewPtr view = nil;
        NSWindowDelegatePtr windowDelegate = nil;
        NSScreenPtr screen = nil;
        CGDirectDisplayID displayId = 0;
        NSUInteger windowStyleMask = 0;
        CGRect windowRect;
    };
}
