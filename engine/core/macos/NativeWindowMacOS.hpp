// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_NATIVEWINDOWMACOS_HPP
#define OUZEL_CORE_NATIVEWINDOWMACOS_HPP

#include <CoreGraphics/CGGeometry.h>
#include <CoreGraphics/CoreGraphics.h>

#ifdef __OBJC__
#  import <Cocoa/Cocoa.h>
using NSWindowPtr = NSWindow*;
using NSViewPtr = NSView*;
using NSWindowDelegatePtr = id<NSWindowDelegate>;
using NSScreenPtr = NSScreen*;
#else
#  include <objc/NSObjCRuntime.h>
using NSWindowPtr = id;
using NSViewPtr = id;
using NSWindowDelegatePtr = id;
using NSScreenPtr = id;
using CGDirectDisplayID = UInt32;
#endif

#include "../NativeWindow.hpp"
#include "../../graphics/Driver.hpp"

namespace ouzel::core::macos
{
    class NativeWindow final: public core::NativeWindow
    {
    public:
        NativeWindow(const math::Size<std::uint32_t, 2>& newSize,
                     bool newResizable,
                     bool newFullscreen,
                     bool newExclusiveFullscreen,
                     const std::string& newTitle,
                     graphics::Driver graphicsDriver,
                     bool newHighDpi);
        ~NativeWindow() override;

        void close();

        void setSize(const math::Size<std::uint32_t, 2>& newSize);
        void setFullscreen(bool newFullscreen);
        void setTitle(const std::string& newTitle);
        void bringToFront();
        void show();
        void hide();
        void minimize();
        void maximize();
        void restore();

        void handleResize();
        void handleClose();
        void handleMinituarize();
        void handleDeminituarize();
        void handleFullscreenChange(bool newFullscreen);
        void handleScaleFactorChange();
        void handleScreenChange();
        void handleBecomeKeyChange();
        void handleResignKeyChange();

        auto getNativeWindow() const noexcept { return window; }
        auto getNativeView() const noexcept { return view; }
        auto getScreen() const noexcept { return screen; }
        auto getDisplayId() const noexcept { return displayId; }

    private:
        void executeCommand(const Command& command) final;

        NSWindowPtr window = nil;
        NSViewPtr view = nil;
        NSWindowDelegatePtr windowDelegate = nil;
        NSScreenPtr screen = nil;
        CGDirectDisplayID displayId = 0;
        CGRect windowRect;
    };
}

#endif // OUZEL_CORE_NATIVEWINDOWMACOS_HPP
