// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "AppDelegate.h"
#import "ViewController.h"
#import "OpenGLView.h"
#include "Engine.h"
#include "Cache.h"

@implementation AppDelegate

-(BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    ouzel::Engine::getInstance()->init();
    
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
    ouzel::Engine::getInstance()->getCache()->releaseTextures();
}

@end
