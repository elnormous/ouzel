// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowIOS.h"
#import "OpenGLView.h"
#import "MetalView.h"
#include "Engine.h"
#include "Utils.h"

@interface ViewController: UIViewController

@end

@implementation ViewController

-(BOOL)prefersStatusBarHidden
{
    return YES;
}

-(void)textFieldDidChange:(id)sender
{
}

@end

namespace ouzel
{
    WindowIOS::WindowIOS(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle):
        Window(pSize, pResizable, pFullscreen, pTitle)
    {

    }

    WindowIOS::~WindowIOS()
    {
        if (textField) [textField release];
        if (viewController) [viewController release];
        if (view) [view release];
        if (window) [window release];
    }

    bool WindowIOS::init()
    {
        window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

        viewController = [[[ViewController alloc] init] autorelease];
        window.rootViewController = viewController;

        CGRect windowFrame = [window bounds];

        size = Size2(windowFrame.size.width, windowFrame.size.height);
        
        switch (sharedEngine->getRenderer()->getDriver())
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

        textField = [[UITextField alloc] init];
        textField.hidden = YES;
        textField.keyboardType = UIKeyboardTypeDefault;
        [textField addTarget:viewController action:@selector(textFieldDidChange:) forControlEvents:UIControlEventEditingChanged];
        [view addSubview:textField];

        viewController.view = view;

        [window makeKeyAndVisible];

        return Window::init();
    }
}
