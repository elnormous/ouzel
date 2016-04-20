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
    class WindowIOS: public Window
    {
        friend Engine;
    public:
        virtual ~WindowIOS();

        UIWindowPtr getNativeWindow() const { return _window; }
        UIViewPtr getNativeView() const { return _view; }

    protected:
        WindowIOS(const Size2& size, bool resizable, bool fullscreen, const std::string& title, graphics::Renderer::Driver driver);
        virtual bool init() override;

        UIWindowPtr _window = Nil;
        UIViewPtr _view = Nil;
        UIViewControllerPtr _viewController = Nil;
    };
}
