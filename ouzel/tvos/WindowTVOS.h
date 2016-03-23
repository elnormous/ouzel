// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Window.h"

#ifdef __OBJC__
@class UIWindow;
typedef UIWindow* UIWindowPtr;
@class OpenGLView;
typedef OpenGLView* OpenGLViewPtr;
#else
#include <objc/objc.h>
typedef id UIWindowPtr;
typedef id OpenGLViewPtr;
#endif

namespace ouzel
{
    class WindowTVOS: public Window
    {
        friend Engine;
    public:
        virtual ~WindowTVOS();

    protected:
        WindowTVOS(const Size2& size, bool resizable, bool fullscreen, const std::string& title);
        virtual bool init() override;

        UIWindowPtr _window;
        OpenGLViewPtr _openGLView;
    };
}
