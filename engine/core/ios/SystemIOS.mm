// Ouzel by Elviss Strazdins

#include <cstdlib>
#import <UIKit/UIKit.h>
#include "SystemIOS.hpp"
#include "../../platform/foundation/AutoreleasePool.hpp"
#include "../../utils/Log.hpp"

ouzel::core::ios::System* systemPointer;

@interface AppDelegate: UIResponder<UIApplicationDelegate>
@end

@implementation AppDelegate

- (BOOL)application:(__unused UIApplication*)application willFinishLaunchingWithOptions:(__unused NSDictionary*)launchOptions
{
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(deviceOrientationDidChange:)
                                                 name:UIDeviceOrientationDidChangeNotification
                                               object:[UIDevice currentDevice]];

    if (systemPointer) systemPointer->start();

    return YES;
}

- (BOOL)application:(__unused UIApplication*)application didFinishLaunchingWithOptions:(__unused NSDictionary*)launchOptions
{
    if (ouzel::engine) ouzel::engine->start();

    return YES;
}

- (void)applicationDidBecomeActive:(__unused UIApplication*)application
{
    if (ouzel::engine) ouzel::engine->resume();
}

- (void)applicationWillResignActive:(__unused UIApplication*)application
{
    if (ouzel::engine) ouzel::engine->pause();
}

- (void)applicationDidEnterBackground:(__unused UIApplication*)application
{
}

- (void)applicationWillEnterForeground:(__unused UIApplication*)application
{
}

- (void)applicationWillTerminate:(__unused UIApplication*)application
{
    if (ouzel::engine) ouzel::engine->exit();
}

- (void)applicationDidReceiveMemoryWarning:(__unused UIApplication*)application
{
    if (ouzel::engine)
    {
        auto event = std::make_unique<ouzel::SystemEvent>();
        event->type = ouzel::Event::Type::lowMemory;

        ouzel::engine->getEventDispatcher().postEvent(std::move(event));
    }
}

- (void)deviceOrientationDidChange:(NSNotification*)note
{
    UIDevice* device = note.object;
    const UIDeviceOrientation orientation = device.orientation;

    auto event = std::make_unique<ouzel::SystemEvent>();
    event->type = ouzel::Event::Type::orientationChange;

    switch (orientation)
    {
        case UIDeviceOrientationPortrait:
            event->orientation = ouzel::SystemEvent::Orientation::portrait;
            break;
        case UIDeviceOrientationPortraitUpsideDown:
            event->orientation = ouzel::SystemEvent::Orientation::portraitReverse;
            break;
        case UIDeviceOrientationLandscapeLeft:
            event->orientation = ouzel::SystemEvent::Orientation::landscape;
            break;
        case UIDeviceOrientationLandscapeRight:
            event->orientation = ouzel::SystemEvent::Orientation::landscapeReverse;
            break;
        case UIDeviceOrientationFaceUp:
            event->orientation = ouzel::SystemEvent::Orientation::faceUp;
            break;
        case UIDeviceOrientationFaceDown:
            event->orientation = ouzel::SystemEvent::Orientation::faceDown;
            break;
        default: // unsupported orientation, assume portrait
            event->orientation = ouzel::SystemEvent::Orientation::portrait;
            break;
    }

    ouzel::engine->getEventDispatcher().postEvent(std::move(event));
}
@end

int main(int argc, char* argv[])
{
    try
    {
        ouzel::platform::foundation::AutoreleasePool autoreleasePool;
        
        ouzel::core::ios::System system{argc, argv};
        systemPointer = &system;
        return system.run();
    }
    catch (const std::exception& e)
    {
        ouzel::log(ouzel::Log::Level::error) << e.what();
        return EXIT_FAILURE;
    }
}

namespace ouzel::core::ios
{
    namespace
    {
        std::vector<std::string> parseArgs(int argc, char* argv[])
        {
            std::vector<std::string> result;
            for (int i = 0; i < argc; ++i)
                result.push_back(argv[i]);
            return result;
        }
    }

    System::System(int argc, char* argv[]):
        core::System{parseArgs(argc, argv)},
        argumentCount{argc},
        arguments{argv}
    {
    }

    int System::run()
    {
        return UIApplicationMain(argumentCount, arguments, nil, NSStringFromClass([AppDelegate class]));
    }

    void System::start()
    {
        engine = std::make_unique<ouzel::core::ios::Engine>(getArgs());
    }
}
