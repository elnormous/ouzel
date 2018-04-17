// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <UIKit/UIKit.h>
#include "EngineIOS.hpp"
#include "thread/Lock.hpp"

@interface AppDelegate: UIResponder<UIApplicationDelegate>

@end

@implementation AppDelegate

-(BOOL)application:(__unused UIApplication*)application willFinishLaunchingWithOptions:(__unused NSDictionary*)launchOptions
{
    ouzel::engine->init();

    return YES;
}

-(BOOL)application:(__unused UIApplication*)application didFinishLaunchingWithOptions:(__unused NSDictionary*)launchOptions
{
    if (ouzel::engine)
    {
        ouzel::engine->start();
    }

    return YES;
}

-(void)applicationDidBecomeActive:(__unused UIApplication*)application
{
    ouzel::engine->resume();
}

-(void)applicationWillResignActive:(__unused UIApplication*)application
{
    ouzel::engine->pause();
}

-(void)applicationDidEnterBackground:(__unused UIApplication*)application
{
}

-(void)applicationWillEnterForeground:(__unused UIApplication*)application
{
}

-(void)applicationWillTerminate:(__unused UIApplication*)application
{
    ouzel::engine->exit();
}

-(void)applicationDidReceiveMemoryWarning:(__unused UIApplication*)application
{
    if (ouzel::engine)
    {
        ouzel::Event event;
        event.type = ouzel::Event::Type::LOW_MEMORY;

        ouzel::engine->getEventDispatcher()->postEvent(event);
    }
}

-(void)executeAll
{
    ouzel::EngineIOS* engineIOS = static_cast<ouzel::EngineIOS*>(ouzel::engine);

    engineIOS->executeAll();
}

@end

namespace ouzel
{
    EngineIOS::EngineIOS(int initArgc, char* initArgv[]):
        argc(initArgc), argv(initArgv)
    {
        for (int i = 0; i < initArgc; ++i)
        {
            args.push_back(initArgv[i]);
        }
    }

    int EngineIOS::run()
    {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        int ret = UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
        [pool release];

        return ret;
    }

    void EngineIOS::executeOnMainThread(const std::function<void(void)>& func)
    {
        Lock lock(executeMutex);

        executeQueue.push(func);

        UIApplication* application = [UIApplication sharedApplication];
        NSObject* delegate = application.delegate;
        [delegate performSelectorOnMainThread:@selector(executeAll) withObject:nil waitUntilDone:NO];
    }

    bool EngineIOS::openURL(const std::string& url)
    {
        NSString* nsStringURL = [NSString stringWithUTF8String:url.c_str()];
        NSURL* nsURL = [NSURL URLWithString:nsStringURL];

        return [[UIApplication sharedApplication] openURL:nsURL] == YES;
    }

    void EngineIOS::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        Engine::setScreenSaverEnabled(newScreenSaverEnabled);

        executeOnMainThread([newScreenSaverEnabled]() {
            [UIApplication sharedApplication].idleTimerDisabled = newScreenSaverEnabled ? YES : NO;
        });
    }

    void EngineIOS::executeAll()
    {
        std::function<void(void)> func;

        for (;;)
        {
            {
                Lock lock(executeMutex);

                if (executeQueue.empty())
                {
                    break;
                }

                func = std::move(executeQueue.front());
                executeQueue.pop();
            }

            if (func)
            {
                func();
            }
        }
    }
}
