// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowIOS.h"
#import "OpenGLView.h"
#import "MetalView.h"
#include "Utils.h"

@interface ViewController: UIViewController

@end

@implementation ViewController

-(BOOL)prefersStatusBarHidden
{
    return YES;
}

@end

namespace ouzel
{
    WindowIOS::WindowIOS(const Size2& size, bool resizable, bool fullscreen, const std::string& title, graphics::Renderer::Driver driver):
        Window(size, resizable, fullscreen, title, driver)
    {

    }

    WindowIOS::~WindowIOS()
    {
        if (_viewController) [_viewController release];
        if (_view) [_view release];
        if (_window) [_window release];
    }

    bool WindowIOS::init()
    {
        _window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

        _viewController = [[[ViewController alloc] init] autorelease];
        _window.rootViewController = _viewController;

        CGRect windowFrame = [_window bounds];

        _size = Size2(windowFrame.size.width, windowFrame.size.height);
        
        switch (_driver)
        {
            case graphics::Renderer::Driver::OPENGL:
                _view = [[OpenGLView alloc] initWithFrame:windowFrame];
                break;
            case graphics::Renderer::Driver::METAL:
                _view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
            default:
                log("Unsupported render driver");
                return false;
        }

        _viewController.view = _view;

        [_window makeKeyAndVisible];

        return Window::init();
    }
}
