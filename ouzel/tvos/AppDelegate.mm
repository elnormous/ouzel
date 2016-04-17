// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "AppDelegate.h"
#include "Engine.h"
#include "Cache.h"
#include "Utils.h"

@implementation AppDelegate

-(BOOL)application:(UIApplication*)application willFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
    OUZEL_UNUSED(application);
    OUZEL_UNUSED(launchOptions);
    ouzelMain(ouzel::getArgs());
    ouzel::sharedEngine->begin();
    return YES;
}

-(BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
    OUZEL_UNUSED(application);
    OUZEL_UNUSED(launchOptions);
    return YES;
}

-(void)applicationWillResignActive:(UIApplication*)application
{
    OUZEL_UNUSED(application);
}

-(void)applicationDidEnterBackground:(UIApplication*)application
{
    OUZEL_UNUSED(application);
}

-(void)applicationWillEnterForeground:(UIApplication*)application
{
    OUZEL_UNUSED(application);
}

-(void)applicationDidBecomeActive:(UIApplication*)application
{
    OUZEL_UNUSED(application);
}

-(void)applicationWillTerminate:(UIApplication*)application
{
    OUZEL_UNUSED(application);
    ouzel::sharedEngine->end();
}

-(void)applicationDidReceiveMemoryWarning:(UIApplication*)application
{
    OUZEL_UNUSED(application);
    ouzel::SystemEventPtr event = std::make_shared<ouzel::SystemEvent>();
    event->type = ouzel::Event::Type::LOW_MEMORY;

    ouzel::sharedEngine->getEventDispatcher()->dispatchEvent(event, ouzel::sharedEngine->getInput());
}

@end
