// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <UIKit/UIKit.h>
#include "EngineIOS.hpp"

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

        mainQueue = dispatch_get_main_queue();
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
        if (func)
        {
            std::function<void(void)> localFunction = func;

            dispatch_async(mainQueue, ^{
                localFunction();
            });
        }
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

        dispatch_async(mainQueue, ^{
            [UIApplication sharedApplication].idleTimerDisabled = newScreenSaverEnabled ? YES : NO;
        });
    }
}
