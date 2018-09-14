// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "NativeWindowIOS.hpp"
#include "ViewIOS.h"
#include "graphics/RenderDevice.hpp"
#include "graphics/opengl/ios/OpenGLView.h"
#include "graphics/metal/ios/MetalView.h"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "utils/Errors.hpp"

@interface ViewController: UIViewController
{
    ouzel::NativeWindowIOS* window;
}

@end

@implementation ViewController

-(id)initWithWindow:(ouzel::NativeWindowIOS*)initWindow
{
    if (self = [super init])
        window = initWindow;

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
            event.systemEvent.orientation = ouzel::SystemEvent::Orientation::PORTRAIT_REVERSE;
            break;
        case UIDeviceOrientationLandscapeLeft:
            event.systemEvent.orientation = ouzel::SystemEvent::Orientation::LANDSCAPE;
            break;
        case UIDeviceOrientationLandscapeRight:
            event.systemEvent.orientation = ouzel::SystemEvent::Orientation::LANDSCAPE_REVERSE;
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

    ouzel::engine->getEventDispatcher().postEvent(event);

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
    NativeWindowIOS::NativeWindowIOS(const std::string& newTitle,
                                     graphics::Renderer::Driver graphicsDriver,
                                     bool newHighDpi):
        ouzel::NativeWindow(Size2(),
                            true,
                            true,
                            true,
                            newTitle,
                            newHighDpi)
    {
        screen = [UIScreen mainScreen];

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

        switch (graphicsDriver)
        {
            case graphics::Renderer::Driver::EMPTY:
                view = [[ViewIOS alloc] initWithFrame:windowFrame];
                break;
#if OUZEL_COMPILE_OPENGL
            case graphics::Renderer::Driver::OPENGL:
                view = [[OpenGLView alloc] initWithFrame:windowFrame];
                break;
#endif
#if OUZEL_COMPILE_METAL
            case graphics::Renderer::Driver::METAL:
                view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
#endif
            default:
                throw SystemError("Unsupported render driver");
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
            resolution = size * contentScale;
        }
        else
            resolution = size;
    }

    NativeWindowIOS::~NativeWindowIOS()
    {
        if (textField) [textField release];
        if (viewController) [viewController release];
        if (view) [view release];
        if (window) [window release];
    }

    void NativeWindowIOS::handleResize(const Size2& newSize)
    {
        size = newSize;
        resolution = size * contentScale;

        addEvent(Event(Event::Type::SIZE_CHANGE, size));
        addEvent(Event(Event::Type::RESOLUTION_CHANGE, resolution));
    }
}
