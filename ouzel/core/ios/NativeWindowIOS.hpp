// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/NativeWindow.hpp"

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

namespace ouzel
{
    class Window;

    class NativeWindowIOS: public NativeWindow
    {
        friend Window;
    public:
        virtual ~NativeWindowIOS();

        inline UIWindowPtr getNativeWindow() const { return window; }
        inline UIViewPtr getNativeView() const { return view; }
        inline UITextFieldPtr getTextField() const { return textField; }

        void handleResize(const Size2& newSize);

    protected:
        NativeWindowIOS();
        virtual void init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth) override;

        UIScreenPtr screen = nil;
        UIWindowPtr window = nil;
        UIViewPtr view = nil;
        UIViewControllerPtr viewController = nil;

        UITextFieldPtr textField = nil;
    };
}
