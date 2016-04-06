// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowIOS.h"
#import "OpenGLView.h"
#import "ViewController.h"

namespace ouzel
{
    WindowIOS::WindowIOS(const Size2& size, bool resizable, bool fullscreen, const std::string& title, video::Renderer::Driver driver):
        Window(size, resizable, fullscreen, title, driver)
    {

    }

    WindowIOS::~WindowIOS()
    {
        [_openGLView release];
        [_window release];
    }

    bool WindowIOS::init()
    {
        _window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

        UIViewController* viewController = [[[ViewController alloc] init] autorelease];
        _window.rootViewController = viewController;

        _openGLView = [[OpenGLView alloc] initWithFrame:[_window bounds]];
        viewController.view = _openGLView;

        [_window makeKeyAndVisible];

        _size.width = _openGLView.backingWidth;
        _size.height = _openGLView.backingHeight;

        [_openGLView prepareOpenGL];

        return Window::init();
    }
}
