// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowTVOS.h"
#import "OpenGLView.h"
#import "MetalView.h"
#include "Utils.h"

@interface ViewController: UIViewController

@end

@implementation ViewController

@end

namespace ouzel
{
    WindowTVOS::WindowTVOS(const Size2& size, bool resizable, bool fullscreen, const std::string& title, video::Renderer::Driver driver):
        Window(size, resizable, fullscreen, title, driver)
    {

    }

    WindowTVOS::~WindowTVOS()
    {
        if (_viewController) [_viewController release];
        if (_view) [_view release];
        if (_window) [_window release];
    }

    bool WindowTVOS::init()
    {
        _window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

        _viewController = [[[ViewController alloc] init] autorelease];
        _window.rootViewController = _viewController;

        CGRect windowFrame = [_window bounds];

        _size = Size2(windowFrame.size.width, windowFrame.size.height);

        switch (_driver)
        {
            case video::Renderer::Driver::OPENGL:
                _view = [[OpenGLView alloc] initWithFrame:windowFrame];
                break;
            case video::Renderer::Driver::METAL:
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
