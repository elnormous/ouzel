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
    WindowTVOS::WindowTVOS(const Size2& size, bool resizable, bool fullscreen, const std::string& title, graphics::Renderer::Driver driver):
        Window(size, resizable, fullscreen, title, driver)
    {

    }

    WindowTVOS::~WindowTVOS()
    {
        if (viewController) [viewController release];
        if (view) [view release];
        if (window) [window release];
    }

    bool WindowTVOS::init()
    {
        window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

        viewController = [[[ViewController alloc] init] autorelease];
        window.rootViewController = viewController;

        CGRect windowFrame = [window bounds];

        size = Size2(windowFrame.size.width, windowFrame.size.height);

        switch (driver)
        {
            case graphics::Renderer::Driver::OPENGL:
                view = [[OpenGLView alloc] initWithFrame:windowFrame];
                break;
            case graphics::Renderer::Driver::METAL:
                view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
            default:
                log("Unsupported render driver");
                return false;
        }

        viewController.view = view;

        [window makeKeyAndVisible];

        return Window::init();
    }
}
