// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "NativeWindowIOS.hpp"
#include "ViewIOS.h"
#include "graphics/RenderDevice.hpp"
#include "graphics/opengl/ios/OpenGLView.h"
#include "graphics/metal/ios/MetalView.h"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"

@interface ViewController: UIViewController

@end

@implementation ViewController
{
    ouzel::NativeWindowIOS* window;
}

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

    window->handleResize(ouzel::Size<2, uint32_t>(static_cast<uint32_t>(size.width),
                                                  static_cast<uint32_t>(size.height)));
}

@end

namespace ouzel
{
    NativeWindowIOS::NativeWindowIOS(const std::function<void(const Event&)>& initCallback,
                                     const std::string& newTitle,
                                     graphics::Driver graphicsDriver,
                                     bool newHighDpi):
        NativeWindow(initCallback,
                     Size<2, uint32_t>(),
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

        CGRect windowFrame = [window bounds];

        size = Size<2, uint32_t>(static_cast<uint32_t>(windowFrame.size.width),
                                 static_cast<uint32_t>(windowFrame.size.height));

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
                throw std::runtime_error("Unsupported render driver");
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
            resolution = size * static_cast<uint32_t>(contentScale);
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
                throw std::runtime_error("Invalid command");
        }
    }

    void NativeWindowIOS::handleResize(const Size<2, uint32_t>& newSize)
    {
        size = newSize;
        resolution = size * static_cast<uint32_t>(contentScale);

        Event sizeChangeEvent(Event::Type::SIZE_CHANGE);
        sizeChangeEvent.size = size;
        sendEvent(sizeChangeEvent);

        Event resolutionChangeEvent(Event::Type::RESOLUTION_CHANGE);
        resolutionChangeEvent.resolution = resolution;
        sendEvent(resolutionChangeEvent);
    }
}
