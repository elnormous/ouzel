// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "AppDelegate.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "core/Cache.h"
#include "events/EventDispatcher.h"
#include "utils/Utils.h"

@implementation AppDelegate

-(BOOL)application:(__unused UIApplication*)application willFinishLaunchingWithOptions:(__unused NSDictionary*)launchOptions
{
    ouzelMain(ouzel::sharedApplication->getArgs());

    if (ouzel::sharedEngine)
    {
        ouzel::sharedEngine->begin();
    }
    return YES;
}

-(BOOL)application:(__unused UIApplication*)application didFinishLaunchingWithOptions:(__unused NSDictionary*)launchOptions
{
    return YES;
}

-(void)applicationDidBecomeActive:(__unused UIApplication*)application
{
    ouzel::sharedEngine->resume();
}

-(void)applicationWillResignActive:(__unused UIApplication*)application
{
    ouzel::sharedEngine->pause();
}

-(void)applicationDidEnterBackground:(__unused UIApplication*)application
{
}

-(void)applicationWillEnterForeground:(__unused UIApplication*)application
{
}

-(void)applicationWillTerminate:(__unused UIApplication*)application
{
    if (ouzel::sharedEngine)
    {
        ouzel::sharedEngine->end();
    }
}

-(void)applicationDidReceiveMemoryWarning:(__unused UIApplication*)application
{
    if (ouzel::sharedEngine)
    {
        ouzel::Event event;
        event.type = ouzel::Event::Type::LOW_MEMORY;

        ouzel::sharedEngine->getEventDispatcher()->postEvent(event);
    }
}

@end
