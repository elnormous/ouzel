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

    window->handleResize(ouzel::Size2(static_cast<float>(size.width),
                                      static_cast<float>(size.height)));
}

-(void)deviceOrientationDidChange:(NSNotification*)note
{
    UIDevice* device = note.object;
    UIDeviceOrientation orientation = device.orientation;

    std::unique_ptr<ouzel::SystemEvent> event(new ouzel::SystemEvent());
    event->type = ouzel::Event::Type::ORIENTATION_CHANGE;

    switch (orientation)
    {
        case UIDeviceOrientationPortrait:
            event->orientation = ouzel::SystemEvent::Orientation::PORTRAIT;
            break;
        case UIDeviceOrientationPortraitUpsideDown:
            event->orientation = ouzel::SystemEvent::Orientation::PORTRAIT_REVERSE;
            break;
        case UIDeviceOrientationLandscapeLeft:
            event->orientation = ouzel::SystemEvent::Orientation::LANDSCAPE;
            break;
        case UIDeviceOrientationLandscapeRight:
            event->orientation = ouzel::SystemEvent::Orientation::LANDSCAPE_REVERSE;
            break;
        case UIDeviceOrientationFaceUp:
            event->orientation = ouzel::SystemEvent::Orientation::FACE_UP;
            break;
        case UIDeviceOrientationFaceDown:
            event->orientation = ouzel::SystemEvent::Orientation::FACE_DOWN;
            break;
        default:
            event->orientation = ouzel::SystemEvent::Orientation::UNKNOWN;
            break;
    }

    ouzel::engine->getEventDispatcher().postEvent(std::move(event));
}

@end

namespace ouzel
{
    NativeWindowIOS::NativeWindowIOS(const std::function<void(const Event&)>& initCallback,
                                     const std::string& newTitle,
                                     graphics::Driver graphicsDriver,
                                     bool newHighDpi):
        NativeWindow(initCallback,
                     Size2(),
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
            case graphics::Driver::EMPTY:
                view = [[ViewIOS alloc] initWithFrame:windowFrame];
                break;
#if OUZEL_COMPILE_OPENGL
            case graphics::Driver::OPENGL:
                view = [[OpenGLView alloc] initWithFrame:windowFrame];
                break;
#endif
#if OUZEL_COMPILE_METAL
            case graphics::Driver::METAL:
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

    void NativeWindowIOS::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::CHANGE_SIZE:
                break;
            case Command::Type::CHANGE_FULLSCREEN:
                // TODO: implement (hide status bar)
                break;
            case Command::Type::CLOSE:
                break;
            case Command::Type::SET_TITLE:
                break;
            default:
                throw SystemError("Invalid command");
        }
    }

    void NativeWindowIOS::handleResize(const Size2& newSize)
    {
        size = newSize;
        resolution = size * contentScale;

        Event sizeChangeEvent(Event::Type::SIZE_CHANGE);
        sizeChangeEvent.size = size;
        sendEvent(sizeChangeEvent);

        Event resolutionChangeEvent(Event::Type::RESOLUTION_CHANGE);
        resolutionChangeEvent.resolution = resolution;
        sendEvent(resolutionChangeEvent);
    }
}
