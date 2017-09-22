// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Window.hpp"

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
        WindowIOS();
        virtual bool init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth) override;

        UIWindowPtr window = nil;
        UIViewPtr view = nil;
        UIViewControllerPtr viewController = nil;

        UITextFieldPtr textField = nil;
    };
}
