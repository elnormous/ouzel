// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <UIKit/UIKit.h>
#include "WindowTVOS.h"
#import "OpenGLView.h"
#import "ViewController.h"

namespace ouzel
{
    WindowTVOS::WindowTVOS(const Size2& size, bool resizable, bool fullscreen, const std::string& title):
        Window(size, resizable, fullscreen, title)
    {
        
    }
    
    WindowTVOS::~WindowTVOS()
    {
        [_openGLView dealloc];
        [_window.rootViewController dealloc];
        [_window dealloc];
    }
    
    bool WindowTVOS::init()
    {
        _window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
        
        UIViewController* viewController = [[ViewController alloc] init];
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