// Ouzel by Elviss Strazdins

#import <UIKit/UIKit.h>
#include "EngineTVOS.hpp"

@interface AppDelegate: UIResponder<UIApplicationDelegate>
@end

@implementation AppDelegate

- (BOOL)application:(__unused UIApplication*)application willFinishLaunchingWithOptions:(__unused NSDictionary*)launchOptions
{
    ouzel::engine->init();

    return YES;
}

- (BOOL)application:(__unused UIApplication*)application didFinishLaunchingWithOptions:(__unused NSDictionary*)launchOptions
{
    if (ouzel::engine)
        ouzel::engine->start();

    return YES;
}

- (void)applicationDidBecomeActive:(__unused UIApplication*)application
{
    ouzel::engine->resume();
}

- (void)applicationWillResignActive:(__unused UIApplication*)application
{
    ouzel::engine->pause();
}

- (void)applicationDidEnterBackground:(__unused UIApplication*)application
{
}

- (void)applicationWillEnterForeground:(__unused UIApplication*)application
{
}

- (void)applicationWillTerminate:(__unused UIApplication*)application
{
    ouzel::engine->exit();
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

@interface ExecuteHandler: NSObject
@end

@implementation ExecuteHandler
{
    ouzel::core::tvos::Engine* engine;
}

- (id)initWithEngine:(ouzel::core::tvos::Engine*)initEngine
{
    if (self = [super init])
        engine = initEngine;

    return self;
}

- (void)executeAll
{
    engine->executeAll();
}
@end

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

    Engine::Engine(int argc, char* argv[]):
        core::Engine{parseArgs(argc, argv)},
        argumentCount{argc},
        arguments{argv}
    {
        executeHanlder = [[ExecuteHandler alloc] initWithEngine:this];
    }

    Engine::~Engine()
    {
        if (executeHanlder) [executeHanlder release];
    }

    void Engine::run()
    {
        UIApplicationMain(argumentCount, arguments, nil, NSStringFromClass([AppDelegate class]));
    }

    void Engine::runOnMainThread(const std::function<void()>& func)
    {
        std::unique_lock lock{executeMutex};
        executeQueue.push(func);
        lock.unlock();

        [executeHanlder performSelectorOnMainThread:@selector(executeAll) withObject:nil waitUntilDone:NO];
    }

    void Engine::openUrl(const std::string& url)
    {
        executeOnMainThread([url]() {
            NSString* urlString = [NSString stringWithUTF8String:url.c_str()];
            NSURL* nsUrl = [NSURL URLWithString:urlString];

            [[UIApplication sharedApplication] openURL:nsUrl];
        });
    }

    void Engine::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        core::Engine::setScreenSaverEnabled(newScreenSaverEnabled);

        executeOnMainThread([newScreenSaverEnabled]() {
            [UIApplication sharedApplication].idleTimerDisabled = newScreenSaverEnabled ? YES : NO;
        });
    }

    void Engine::executeAll()
    {
        std::function<void()> func;

        for (;;)
        {
            std::unique_lock lock{executeMutex};

            if (executeQueue.empty()) break;

            func = std::move(executeQueue.front());
            executeQueue.pop();
            lock.unlock();

            if (func) func();
        }
    }
}
