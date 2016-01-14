// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "AppDelegate.h"
#import "ViewController.h"
#import "OpenGLView.h"
#include "Engine.h"

@implementation AppDelegate

-(BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    ouzel::Engine::getInstance()->init();
    
    _window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    UIViewController* viewController = [[ViewController alloc] init];
    _window.rootViewController = viewController;
    
    OpenGLView* view = [[OpenGLView alloc] initWithFrame:[_window bounds]];
    viewController.view = view;
    
    [_window makeKeyAndVisible];
    
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
    ouzel::Engine::getInstance()->getRenderer()->releaseTextures();
}

@end
