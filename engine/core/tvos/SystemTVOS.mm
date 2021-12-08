// Ouzel by Elviss Strazdins

#include <cstdlib>
#import <UIKit/UIKit.h>
#include "SystemTVOS.hpp"
#include "../../platform/foundation/AutoreleasePool.hpp"
#include "../../utils/Log.hpp"

ouzel::core::tvos::System* systemPointer;

@interface AppDelegate: UIResponder<UIApplicationDelegate>
@end

@implementation AppDelegate

- (BOOL)application:(__unused UIApplication*)application willFinishLaunchingWithOptions:(__unused NSDictionary*)launchOptions
{
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
@end

int main(int argc, char* argv[])
{
    try
    {
        ouzel::core::tvos::System system{argc, argv};
        systemPointer = &system;
        return system.run();
    }
    catch (const std::exception& e)
    {
        ouzel::log(ouzel::Log::Level::error) << e.what();
        return EXIT_FAILURE;
    }
}

namespace ouzel::core::tvos
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
        ouzel::platform::foundation::AutoreleasePool autoreleasePool;
        return UIApplicationMain(argumentCount, arguments, nil, NSStringFromClass([AppDelegate class]));
    }

    void System::start()
    {
        engine = std::make_unique<Engine>(getArgs());
    }
}
