// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Window.hpp"

#if defined(__OBJC__)
#import <Cocoa/Cocoa.h>
#import <CoreGraphics/CoreGraphics.h>
typedef NSWindow* NSWindowPtr;
typedef NSView* NSViewPtr;
typedef id<NSWindowDelegate> NSWindowDelegatePtr;
#else
#include <objc/objc.h>
typedef id NSWindowPtr;
typedef id NSViewPtr;
typedef id NSWindowDelegatePtr;
typedef uint32_t CGDirectDisplayID;
#endif

namespace ouzel
{
    class Engine;

    class WindowMacOS: public Window
    {
        friend Engine;
    public:
        virtual ~WindowMacOS();

        virtual void close() override;

        virtual void setSize(const Size2& newSize) override;
        virtual void setFullscreen(bool newFullscreen) override;
        virtual void setTitle(const std::string& newTitle) override;

        void handleResize();
        void handleClose();
        void handleFullscreenChange(bool newFullscreen);
        void handleScaleFactorChange();
        void handleScreenChange();

        NSWindowPtr getNativeWindow() const { return window; }
        NSViewPtr getNativeView() const { return view; }
        CGDirectDisplayID getDisplayId() const { return displayId; }

    protected:
        WindowMacOS();
        virtual bool init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth) override;

        NSWindowPtr window = nil;
        NSViewPtr view = nil;
        NSWindowDelegatePtr windowDelegate = nil;
        CGDirectDisplayID displayId = 0;
    };
}
