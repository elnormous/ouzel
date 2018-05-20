// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <UIKit/UIKit.h>
#include "EngineTVOS.hpp"
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

@end

@interface ExecuteHandler: NSObject
{
    ouzel::EngineTVOS* engine;
}
@end

@implementation ExecuteHandler

-(id)initWithEngine:(ouzel::EngineTVOS*)initEngine
{
    if (self = [super init])
    {
        engine = initEngine;
    }

    return self;
}

-(void)executeAll
{
    engine->executeAll();
}

@end

namespace ouzel
{
    EngineTVOS::EngineTVOS(int initArgc, char* initArgv[]):
        argc(initArgc), argv(initArgv)
    {
        for (int i = 0; i < initArgc; ++i)
        {
            args.push_back(initArgv[i]);
        }

        executeHanlder = [[ExecuteHandler alloc] initWithEngine:this];
    }

    EngineTVOS::~EngineTVOS()
    {
        if (executeHanlder) [executeHanlder release];
    }

    int EngineTVOS::run()
    {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        int ret = UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
        [pool release];

        return ret;
    }

    void EngineTVOS::executeOnMainThread(const std::function<void(void)>& func)
    {
        Lock lock(executeMutex);

        executeQueue.push(func);

        [executeHanlder performSelectorOnMainThread:@selector(executeAll) withObject:nil waitUntilDone:NO];
    }

    bool EngineTVOS::openURL(const std::string& url)
    {
        NSString* nsStringURL = [NSString stringWithUTF8String:url.c_str()];
        NSURL* nsURL = [NSURL URLWithString:nsStringURL];

        return [[UIApplication sharedApplication] openURL:nsURL] == YES;
    }

    void EngineTVOS::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        Engine::setScreenSaverEnabled(newScreenSaverEnabled);

        executeOnMainThread([newScreenSaverEnabled]() {
            [UIApplication sharedApplication].idleTimerDisabled = newScreenSaverEnabled ? YES : NO;
        });
    }

    void EngineTVOS::executeAll()
    {
        std::function<void(void)> func;

        for (;;)
        {
            {
                Lock lock(executeMutex);

                if (executeQueue.empty()) break;

                func = std::move(executeQueue.front());
                executeQueue.pop();
            }

            if (func) func();
        }
    }
}
