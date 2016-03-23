// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "AppDelegate.h"
#import "ViewController.h"
#import "OpenGLView.h"
#include "Engine.h"
#include "Cache.h"
#include "Utils.h"

@implementation AppDelegate

void ouzelMain(const std::vector<std::string>& args);

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
    ouzel::Engine::getInstance()->end();
}

-(void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    ouzel::SystemEventPtr event = std::make_shared<ouzel::SystemEvent>();
    event->type = ouzel::Event::Type::LOW_MEMORY;

    ouzel::Engine::getInstance()->getEventDispatcher()->dispatchEvent(event, ouzel::Engine::getInstance()->getInput());
}

@end
