// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowIOS.h"
#include "graphics/opengl/ios/OpenGLView.h"
#include "graphics/metal/ios/MetalView.h"
#include "core/Engine.h"
#include "events/EventDispatcher.h"
#include "utils/Log.h"

@interface ViewController: UIViewController
{
    ouzel::WindowIOS* window;
}

@end

@implementation ViewController

-(id)initWithWindow:(ouzel::WindowIOS*)newWindow
{
    if (self = [super init])
    {
        window = newWindow;
    }

    return self;
}

-(BOOL)prefersStatusBarHidden
{
    return YES;
}

-(void)textFieldDidChange:(__unused id)sender
{
}

-(NSUInteger)supportedInterfaceOrientations
{
    // TODO: add setting to limit orientations
    return UIInterfaceOrientationMaskAll;
}

-(void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];

    window->setSize(ouzel::Size2(static_cast<float>(size.width),
                                 static_cast<float>(size.height)));
}

-(void)deviceOrientationDidChange:(NSNotification*)note
{
    UIDevice* device = note.object;
    UIDeviceOrientation orientation = device.orientation;

    ouzel::Event event;
    event.type = ouzel::Event::Type::ORIENTATION_CHANGE;

    switch (orientation)
    {
        case UIDeviceOrientationPortrait:
            event.systemEvent.orientation = ouzel::SystemEvent::Orientation::PORTRAIT;
            break;
        case UIDeviceOrientationPortraitUpsideDown:
            event.systemEvent.orientation = ouzel::SystemEvent::Orientation::PORTRAIT_UPSIDE_DOWN;
            break;
        case UIDeviceOrientationLandscapeLeft:
            event.systemEvent.orientation = ouzel::SystemEvent::Orientation::LANDSCAPE_LEFT;
            break;
        case UIDeviceOrientationLandscapeRight:
            event.systemEvent.orientation = ouzel::SystemEvent::Orientation::LANDSCAPE_RIGHT;
            break;
        case UIDeviceOrientationFaceUp:
            event.systemEvent.orientation = ouzel::SystemEvent::Orientation::FACE_UP;
            break;
        case UIDeviceOrientationFaceDown:
            event.systemEvent.orientation = ouzel::SystemEvent::Orientation::FACE_DOWN;
            break;
        default:
            event.systemEvent.orientation = ouzel::SystemEvent::Orientation::UNKNOWN;
            break;
    }

    ouzel::sharedEngine->getEventDispatcher()->postEvent(event);

    if (self.view)
    {
        CGSize size = self.view.frame.size;

        window->handleResize(ouzel::Size2(static_cast<float>(size.width),
                                          static_cast<float>(size.height)));
    }
}

@end

namespace ouzel
{
    WindowIOS::WindowIOS()
    {
    }

    WindowIOS::~WindowIOS()
    {
        if (textField) [textField release];
        if (viewController) [viewController release];
        if (view) [view release];
        if (window) [window release];
    }

    bool WindowIOS::init(const Size2& newSize,
                         bool newResizable,
                         bool newFullscreen,
                         const std::string& newTitle,
                         bool newHighDpi,
                         bool depth)
    {
        if (!Window::init(newSize, newResizable, newFullscreen, newTitle, newHighDpi, depth))
        {
            return false;
        }

        UIScreen* screen = [UIScreen mainScreen];

        window = [[UIWindow alloc] initWithFrame:[screen bounds]];

        viewController = [[[ViewController alloc] initWithWindow:this] autorelease];
        window.rootViewController = viewController;

        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
        [[NSNotificationCenter defaultCenter] addObserver:viewController selector:@selector(deviceOrientationDidChange:)
                                                     name:UIDeviceOrientationDidChangeNotification
                                                   object:[UIDevice currentDevice]];

        CGRect windowFrame = [window bounds];

        size = Size2(static_cast<float>(windowFrame.size.width),
                     static_cast<float>(windowFrame.size.height));

        switch (sharedEngine->getRenderer()->getDriver())
        {
            case graphics::Renderer::Driver::EMPTY:
                view = [[UIView alloc] initWithFrame:windowFrame];
                break;
#if OUZEL_SUPPORTS_OPENGL
            case graphics::Renderer::Driver::OPENGL:
                view = [[OpenGLView alloc] initWithFrame:windowFrame];
                break;
#endif
#if OUZEL_SUPPORTS_METAL
            case graphics::Renderer::Driver::METAL:
                view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
#endif
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

        if (highDpi)
        {
            contentScale = static_cast<float>(screen.scale);
        }

        return true;
    }

    void WindowIOS::handleResize(const Size2& newSize)
    {
        Event event;
        event.type = Event::Type::WINDOW_SIZE_CHANGE;

        event.windowEvent.window = this;
        event.windowEvent.size = newSize;

        sharedEngine->getEventDispatcher()->postEvent(event);
    }
}
