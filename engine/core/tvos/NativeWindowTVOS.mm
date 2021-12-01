// Ouzel by Elviss Strazdins

#include <stdexcept>
#include "NativeWindowTVOS.hpp"
#include "ViewTVOS.h"
#include "../../graphics/RenderDevice.hpp"
#include "../../graphics/opengl/tvos/OpenGLView.h"
#include "../../graphics/metal/tvos/MetalView.h"

@interface ViewController: UIViewController
@end

@implementation ViewController
{
    ouzel::core::tvos::NativeWindow* window;
}

- (id)initWithWindow:(ouzel::core::tvos::NativeWindow*)initWindow
{
    if (self = [super init])
        window = initWindow;

    return self;
}

- (void)textFieldDidChange:(__unused id)sender
{
}

- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];

    window->handleResize(ouzel::math::Size<std::uint32_t, 2>{
        static_cast<std::uint32_t>(size.width),
        static_cast<std::uint32_t>(size.height)
    });
}
@end

namespace ouzel::core::tvos
{
    NativeWindow::NativeWindow(const std::function<void(const Event&)>& initCallback,
                               const std::string& newTitle,
                               graphics::Driver graphicsDriver,
                               bool newHighDpi):
        core::NativeWindow{
            initCallback,
            math::Size<std::uint32_t, 2>{},
            true,
            true,
            true,
            newTitle,
            newHighDpi
        }
    {
        screen = [UIScreen mainScreen];

        window = [[UIWindow alloc] initWithFrame:[screen bounds]];

        viewController = [[[ViewController alloc] initWithWindow:this] autorelease];
        window.rootViewController = viewController;

        const CGRect windowFrame = [window bounds];

        size = math::Size<std::uint32_t, 2>{
            static_cast<std::uint32_t>(windowFrame.size.width),
            static_cast<std::uint32_t>(windowFrame.size.height)
        };

        switch (graphicsDriver)
        {
            case graphics::Driver::empty:
                view = [[View alloc] initWithFrame:windowFrame];
                break;
#if OUZEL_COMPILE_OPENGL
            case graphics::Driver::openGl:
                view = [[OpenGLView alloc] initWithFrame:windowFrame];
                break;
#endif
#if OUZEL_COMPILE_METAL
            case graphics::Driver::metal:
                view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
#endif
            default:
                throw std::runtime_error("Unsupported render driver");
        }

        textField = [[UITextField alloc] init];
        textField.hidden = YES;
        textField.keyboardType = UIKeyboardTypeDefault;
        [textField addTarget:viewController action:@selector(textFieldDidChange:)
            forControlEvents:UIControlEventEditingChanged];
        [view addSubview:textField];

        viewController.view = view;

        [window makeKeyAndVisible];

        if (highDpi)
        {
            contentScale = static_cast<float>(screen.scale);
            resolution = size * static_cast<std::uint32_t>(contentScale);
        }
        else
            resolution = size;
    }

    NativeWindow::~NativeWindow()
    {
        if (textField) [textField release];
        if (viewController) [viewController release];
        if (view) [view release];
        if (window) [window release];
    }

    void NativeWindow::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::changeSize:
                break;
            case Command::Type::changeFullscreen:
                break;
            case Command::Type::close:
                break;
            case Command::Type::setTitle:
                break;
            case Command::Type::bringToFront:
                break;
            case Command::Type::show:
                break;
            case Command::Type::hide:
                break;
            case Command::Type::minimize:
                break;
            case Command::Type::maximize:
                break;
            case Command::Type::restore:
                break;
            default:
                throw std::runtime_error("Invalid command");
        }
    }

    void NativeWindow::handleResize(const math::Size<std::uint32_t, 2>& newSize)
    {
        size = newSize;
        resolution = size * contentScale;

        Event sizeChangeEvent(Event::Type::sizeChange);
        sizeChangeEvent.size = size;
        sendEvent(sizeChangeEvent);

        Event resolutionChangeEvent(Event::Type::resolutionChange);
        resolutionChangeEvent.size = resolution;
        sendEvent(resolutionChangeEvent);
    }
}
