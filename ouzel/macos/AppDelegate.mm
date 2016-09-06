// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "AppDelegate.h"
#import "OpenGLView.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "events/EventDispatcher.h"
#include "graphics/Renderer.h"
#include "WindowMacOS.h"
#include "math/Size2.h"
#include "utils/Utils.h"

@implementation AppDelegate

-(void)applicationWillFinishLaunching:(__unused NSNotification*)notification
{
    ouzelMain(ouzel::sharedApplication->getArgs());

    if (ouzel::sharedEngine)
    {
        ouzel::sharedEngine->begin();
    }
}

-(void)applicationDidFinishLaunching:(__unused NSNotification*)notification
{
}

-(void)applicationWillTerminate:(__unused NSNotification*)notification
{
    if (ouzel::sharedEngine)
    {
        ouzel::sharedEngine->end();
    }
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(__unused NSApplication*)sender
{
    return YES;
}

-(BOOL)application:(__unused NSApplication*)sender openFile:(NSString*)filename
{
    if (ouzel::sharedEngine)
    {
        ouzel::Event event;
        event.type = ouzel::Event::Type::OPEN_FILE;

        event.systemEvent.filename = [filename cStringUsingEncoding:NSUTF8StringEncoding];

        ouzel::sharedEngine->getEventDispatcher()->postEvent(event);
    }

    return YES;
}

-(void)applicationDidBecomeActive:(__unused NSNotification*)notification
{
    ouzel::sharedEngine->resume();
}

-(void)applicationDidResignActive:(__unused NSNotification*)notification
{
    ouzel::sharedEngine->pause();
}

@end
