// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_NATIVEWINDOWIOS_HPP
#define OUZEL_CORE_NATIVEWINDOWIOS_HPP

#if defined(__OBJC__)
#import <UIKit/UIKit.h>
typedef UIScreen* UIScreenPtr;
typedef UIWindow* UIWindowPtr;
typedef UIView* UIViewPtr;
typedef UIViewController* UIViewControllerPtr;
typedef UITextField* UITextFieldPtr;
#else
#include <objc/objc.h>
typedef id UIScreenPtr;
typedef id UIWindowPtr;
typedef id UIViewPtr;
typedef id UIViewControllerPtr;
typedef id UITextFieldPtr;
#endif

#include "core/NativeWindow.hpp"
#include "graphics/Renderer.hpp"

namespace ouzel
{
    class NativeWindowIOS final: public NativeWindow
    {
    public:
        NativeWindowIOS(EventHandler& initEventHandler,
                        const std::string& newTitle,
                        graphics::Driver graphicsDriver,
                        bool newHighDpi);
        ~NativeWindowIOS();

        inline UIWindowPtr getNativeWindow() const { return window; }
        inline UIViewPtr getNativeView() const { return view; }
        inline UITextFieldPtr getTextField() const { return textField; }

        void handleResize(const Size2& newSize);

    private:
        UIScreenPtr screen = nil;
        UIWindowPtr window = nil;
        UIViewPtr view = nil;
        UIViewControllerPtr viewController = nil;

        UITextFieldPtr textField = nil;
    };
}

#endif // OUZEL_CORE_NATIVEWINDOWIOS_HPP
