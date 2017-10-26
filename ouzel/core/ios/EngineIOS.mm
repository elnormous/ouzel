// Copyright (C) 2017 Elviss Strazdins
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
    EngineIOS::EngineIOS(int aArgc, char* aArgv[])
    {
        argc = aArgc;
        argv = aArgv;

        for (int i = 0; i < aArgc; ++i)
        {
            args.push_back(aArgv[i]);
        }

        mainQueue = dispatch_get_main_queue();
    }

    int EngineIOS::run()
    {
        @autoreleasepool
        {
            return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
        }
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

    bool EngineIOS::setCurrentThreadName(const std::string& name)
    {
        [[NSThread currentThread] setName:[NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding]];

        return true;
    }
}
