// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_NATIVEWINDOWMACOS_HPP
#define OUZEL_CORE_NATIVEWINDOWMACOS_HPP

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
    class NativeWindowMacOS final: public NativeWindow
    {
    public:
        NativeWindowMacOS(const std::function<void(const Event&)>& initCallback,
                          const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          graphics::Driver graphicsDriver,
                          bool newHighDpi);
        ~NativeWindowMacOS();

        void close() override;

        void setSize(const Size2& newSize) override;
        void setFullscreen(bool newFullscreen) override;
        void setTitle(const std::string& newTitle) override;

        void handleResize();
        void handleClose();
        void handleFullscreenChange(bool newFullscreen);
        void handleScaleFactorChange();
        void handleScreenChange();

        inline NSWindowPtr getNativeWindow() const { return window; }
        inline NSViewPtr getNativeView() const { return view; }
        inline NSScreenPtr getScreen() const { return screen; }
        inline CGDirectDisplayID getDisplayId() const { return displayId; }

    private:
        NSWindowPtr window = nil;
        NSViewPtr view = nil;
        NSWindowDelegatePtr windowDelegate = nil;
        NSScreenPtr screen = nil;
        CGDirectDisplayID displayId = 0;
        NSUInteger windowStyleMask = 0;
        CGRect windowRect;
    };
}

#endif // OUZEL_CORE_NATIVEWINDOWMACOS_HPP
