// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Window.h"

#if defined(__OBJC__)
#import <UIKit/UIKit.h>
typedef UIWindow* UIWindowPtr;
typedef UIView* UIViewPtr;
typedef UIViewController* UIViewControllerPtr;
typedef UITextField* UITextFieldPtr;
#else
#include <objc/objc.h>
typedef id UIWindowPtr;
typedef id UIViewPtr;
typedef id UIViewControllerPtr;
typedef id UITextFieldPtr;
#endif

namespace ouzel
{
    class WindowIOS: public Window
    {
        friend Engine;
    public:
        virtual ~WindowIOS();

        UIWindowPtr getNativeWindow() const { return window; }
        UIViewPtr getNativeView() const { return view; }
        UITextFieldPtr getTextField() const { return textField; }

        void handleResize(const Size2& newSize);

    protected:
        WindowIOS(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle);
        virtual bool init() override;

        UIWindowPtr window = Nil;
        UIViewPtr view = Nil;
        UIViewControllerPtr viewController = Nil;

        UITextFieldPtr textField = Nil;
    };
}
