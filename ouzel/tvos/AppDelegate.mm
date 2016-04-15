// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "AppDelegate.h"
#import "ViewController.h"
#import "OpenGLView.h"
#include "Engine.h"
#include "Cache.h"
#include "Utils.h"

@implementation AppDelegate

-(BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    ouzelMain(ouzel::getArgs());
    return YES;
}

-(void)applicationWillResignActive:(UIApplication *)application
{

}

-(void)applicationDidEnterBackground:(UIApplication *)application
{

}

-(void)applicationWillEnterForeground:(UIApplication *)application
{

}

-(void)applicationDidBecomeActive:(UIApplication *)application
{

}

-(void)applicationWillTerminate:(UIApplication *)application
{
    ouzel::sharedEngine->end();
}

-(void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    ouzel::SystemEventPtr event = std::make_shared<ouzel::SystemEvent>();
    event->type = ouzel::Event::Type::LOW_MEMORY;

    ouzel::sharedEngine->getEventDispatcher()->dispatchEvent(event, ouzel::sharedEngine->getInput());
}

@end
