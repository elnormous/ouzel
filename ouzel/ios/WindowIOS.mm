// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowIOS.h"
#import "OpenGLView.h"
#import "MetalView.h"
#include "core/Engine.h"
#include "utils/Utils.h"

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

-(NSUInteger)supportedInterfaceOrientations
{
    // TODO: add setting to limit orientations
    return UIInterfaceOrientationMaskAll;
}

-(void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id <UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];

    ouzel::sharedEngine->getWindow()->setSize(ouzel::Size2(static_cast<float>(size.width),
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

    ouzel::sharedEngine->getEventDispatcher()->dispatchEvent(event);

    if (self.view)
    {
        CGSize size = self.view.frame.size;

        ouzel::sharedEngine->getWindow()->setSize(ouzel::Size2(static_cast<float>(size.width),
                                                               static_cast<float>(size.height)));
    }
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

        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
        [[NSNotificationCenter defaultCenter] addObserver:viewController selector:@selector(deviceOrientationDidChange:)
                                                     name:UIDeviceOrientationDidChangeNotification
                                                   object:[UIDevice currentDevice]];

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
