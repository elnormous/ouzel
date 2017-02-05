// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowTVOS.h"
#import "graphics/opengl/ios/OpenGLView.h"
#import "graphics/metal/ios/MetalView.h"
#include "core/Engine.h"
#include "utils/Log.h"

@interface ViewController: UIViewController
{
    ouzel::WindowTVOS* window;
}

@end

@implementation ViewController

-(id)initWithWindow:(ouzel::WindowTVOS*)newWindow
{
    if (self = [super init])
    {
        window = newWindow;
    }

    return self;
}

-(void)textFieldDidChange:(__unused id)sender
{
}

-(void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];

    window->handleResize(ouzel::Size2(static_cast<float>(size.width),
                                      static_cast<float>(size.height)));
}

@end

namespace ouzel
{
    WindowTVOS::WindowTVOS(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle):
        Window(aSize, aResizable, aFullscreen, aTitle)
    {
    }

    WindowTVOS::~WindowTVOS()
    {
        if (textField) [textField release];
        if (viewController) [viewController release];
        if (view) [view release];
        if (window) [window release];
    }

    bool WindowTVOS::init()
    {
        UIScreen* screen = [UIScreen mainScreen];

        window = [[UIWindow alloc] initWithFrame:[screen bounds]];

        viewController = [[[ViewController alloc] initWithWindow:this] autorelease];
        window.rootViewController = viewController;

        CGRect windowFrame = [window bounds];

        size = Size2(static_cast<float>(windowFrame.size.width),
                     static_cast<float>(windowFrame.size.height));

        switch (sharedEngine->getRenderer()->getDriver())
        {
            case graphics::Renderer::Driver::EMPTY:
                view = [[UIView alloc] initWithFrame:windowFrame];
                break;
            case graphics::Renderer::Driver::OPENGL:
                view = [[OpenGLView alloc] initWithFrame:windowFrame];
                break;
            case graphics::Renderer::Driver::METAL:
                view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
            default:
                Log(Log::Level::ERR) << "Unsupported render driver";
                return false;
        }

        textField = [[UITextField alloc] init];
        textField.hidden = YES;
        textField.keyboardType = UIKeyboardTypeDefault;
        [textField addTarget:viewController action:@selector(textFieldDidChange:) forControlEvents:UIControlEventEditingChanged];
        [view addSubview:textField];

        viewController.view = view;

        [window makeKeyAndVisible];

        contentScale = static_cast<float>(screen.scale);

        return Window::init();
    }

    void WindowTVOS::handleResize(const Size2& newSize)
    {
        Event event;
        event.type = Event::Type::WINDOW_SIZE_CHANGE;

        event.windowEvent.window = this;
        event.windowEvent.size = newSize;

        sharedEngine->getEventDispatcher()->postEvent(event);
    }
}
