// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Window.h"

#if defined(__OBJC__)
#import <Cocoa/Cocoa.h>
typedef NSWindow* NSWindowPtr;
typedef NSView* NSViewPtr;
typedef id<NSWindowDelegate> NSWindowDelegatePtr;
#else
#include <objc/objc.h>
typedef id NSWindowPtr;
typedef id NSViewPtr;
typedef id NSWindowDelegatePtr;
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

        virtual float getContentScale() const override;

        void handleResize();
        void handleClose();
        void handleFullscreenChange(bool fullscreen);

        NSWindowPtr getNativeWindow() const { return window; }
        NSViewPtr getNativeView() const { return view; }

    protected:
        WindowMacOS(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle);
        virtual bool init() override;

        NSWindowPtr window = Nil;
        NSViewPtr view = Nil;
        NSWindowDelegatePtr windowDelegate = Nil;
    };
}
