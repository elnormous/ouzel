// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_NATIVEWINDOWIOS_HPP
#define OUZEL_CORE_NATIVEWINDOWIOS_HPP

#if defined(__OBJC__)
#  import <UIKit/UIKit.h>
typedef UIScreen* UIScreenPtr;
typedef UIWindow* UIWindowPtr;
typedef UIView* UIViewPtr;
typedef UIViewController* UIViewControllerPtr;
typedef UITextField* UITextFieldPtr;
#else
#  include <objc/objc.h>
typedef id UIScreenPtr;
typedef id UIWindowPtr;
typedef id UIViewPtr;
typedef id UIViewControllerPtr;
typedef id UITextFieldPtr;
#endif

#include "../NativeWindow.hpp"
#include "../../graphics/Renderer.hpp"

namespace ouzel
{
    class NativeWindowIOS final: public NativeWindow
    {
    public:
        NativeWindowIOS(const std::function<void(const Event&)>& initCallback,
                        const std::string& newTitle,
                        graphics::Driver graphicsDriver,
                        bool newHighDpi);
        ~NativeWindowIOS() override;

        void executeCommand(const Command& command) final;

        auto getNativeWindow() const noexcept { return window; }
        auto getNativeView() const noexcept { return view; }
        auto getTextField() const noexcept { return textField; }

        void handleResize(const Size2U& newSize);

    private:
        UIScreenPtr screen = nil;
        UIWindowPtr window = nil;
        UIViewPtr view = nil;
        UIViewControllerPtr viewController = nil;

        UITextFieldPtr textField = nil;
    };
}

#endif // OUZEL_CORE_NATIVEWINDOWIOS_HPP
