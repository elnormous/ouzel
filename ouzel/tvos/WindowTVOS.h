// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Window.h"

#ifdef __OBJC__
#import <UIKit/UIKit.h>
typedef UIWindow* UIWindowPtr;
typedef UIView* UIViewPtr;
typedef UIViewController* UIViewControllerPtr;
#else
#include <objc/objc.h>
typedef id UIWindowPtr;
typedef id UIViewPtr;
typedef id UIViewControllerPtr;
#endif

namespace ouzel
{
    class WindowTVOS: public Window
    {
        friend Engine;
    public:
        virtual ~WindowTVOS();

        UIWindowPtr getNativeWindow() const { return window; }
        UIViewPtr getNativeView() const { return view; }

    protected:
        WindowTVOS(const Size2& size, bool resizable, bool fullscreen, const std::string& title, graphics::Renderer::Driver driver);
        virtual bool init() override;

        UIWindowPtr window = Nil;
        UIViewPtr view = Nil;
        UIViewControllerPtr viewController = Nil;
    };
}
